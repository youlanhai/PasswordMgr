// PasswordMgr.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "..\PwdLib\PwdMgr.h"

#include <Windows.h>
#include <algorithm>
#include <iostream>
#include <cassert>


typedef std::vector<std::wstring> StringVector;

inline bool isWhiteSpace(wchar_t ch)
{
    return ch == 0 || ch == L' ' || ch == L'\t' || ch == L'\r' || ch == L'\n';
}

//解析命令行参数
void parseCMDLine(StringVector & argv, const std::wstring & cmd)
{
	argv.clear();

	std::wstring temp;
	temp.reserve(256);

    bool noQuot = true;

	for (size_t i = 0; i < cmd.size(); ++i)
	{
		wchar_t ch = cmd[i];
		if (isWhiteSpace(ch) && noQuot)
		{
			if (!temp.empty())
			{
				argv.push_back(temp);
				temp.clear();
			}
		}
		else if(ch == L'"')
        {
            if(noQuot) noQuot = false;
            else
            {
                noQuot = true;
                argv.push_back(temp);
				temp.clear();
            }
        }
        else
		{
			temp += ch;
		}
	}

	if (!temp.empty())
		argv.push_back(temp);
}


FILE* openFile(const std::wstring & fname, const std::wstring & mode)
{
    FILE * pFile = NULL;
    _wfopen_s(&pFile, fname.c_str(), mode.c_str());

    return pFile;
}

long flength(FILE* pFile)
{
    assert(pFile != NULL);

    long cur = ftell(pFile);
    fseek(pFile, 0, SEEK_END);
    long length = ftell(pFile);
    fseek(pFile, cur, SEEK_SET);
    return length;
}

//将当前路径设置为应用程序所在目录
void resetcwd()
{
    std::wstring temp(MAX_PATH, 0);
    GetModuleFileNameW(NULL, &temp[0], MAX_PATH);
    std::replace(temp.begin(), temp.end(), L'\\', L'/');
    size_t pos = temp.find_last_of(L'/');
    if(pos != temp.npos)
    {
        temp.erase(pos, temp.npos);
        SetCurrentDirectoryW(temp.c_str());
        std::wcout<<L"reset cur dir to: "<<temp<<std::endl;
    }
    else
    {
        std::wcout<<L"reset cur dir failed!"<<std::endl;
    }
}

bool ensure(const std::wstring & content, wchar_t yes=L'y', wchar_t no=L'n')
{
    std::wcout<<content<<L"[" <<yes <<L"/" <<no <<L"]";

    wchar_t ch;
    do
    {
        std::wcin>>ch;
    }while(ch != yes && ch != no);

    return ch == yes;
}

//////////////////////////////////////////////////

struct CMDNode;
typedef void (*CMDFun)(pwd::PwdMgr & pwdmgr, const StringVector & argv, const CMDNode *userCmdMap);

struct CMDNode
{
    CMDNode(const std::wstring & cmd, CMDFun fun, const std::wstring & desc)
       : cmd_(cmd), fun_(fun), desc_(desc)
    {}

    void operator()(pwd::PwdMgr & pwdmgr, const StringVector & argv, const CMDNode *userCmdMap) const
    {
        if (fun_) fun_(pwdmgr, argv, userCmdMap);
    }

    std::wstring    cmd_;
    CMDFun          fun_;
    std::wstring    desc_;
};

const CMDNode* findCMD(const CMDNode *cmdMap, const std::wstring & cmd)
{
    assert(cmdMap != NULL);

    while (!cmdMap->cmd_.empty())
    {
        if (cmdMap->cmd_ == cmd)
        {
            return cmdMap;
        }

        ++cmdMap;
    }

    return NULL;
}

//辅助宏

//命令函数声明
#define DECLARE_CMDFUN(name) void name(pwd::PwdMgr & pwdmgr, const StringVector & argv, const CMDNode *userCmdMap)

//显示帮助信息
#define HOOK_HELP(cmd)                                  \
if (argv.size() <= 1)                                   \
{                                                       \
    const CMDNode *pNode = findCMD(userCmdMap, L#cmd);  \
    if (!pNode) pNode = findCMD(g_cmdMap, L#cmd);       \
    if (pNode) std::wcout << pNode->desc_ << std::endl; \
    return;                                             \
}

//命令函数申明
DECLARE_CMDFUN(add);
DECLARE_CMDFUN(del);
DECLARE_CMDFUN(help);
DECLARE_CMDFUN(load);
DECLARE_CMDFUN(save);
DECLARE_CMDFUN(view);
DECLARE_CMDFUN(modify);
DECLARE_CMDFUN(search);
DECLARE_CMDFUN(userLoad);
DECLARE_CMDFUN(userSave);
DECLARE_CMDFUN(userLoadBackup);
DECLARE_CMDFUN(userBackup);

void _view(const pwd::Pwd & data);
void _add(pwd::Pwd & data);
bool _doLoad(pwd::PwdMgr & pwdmgr, const std::wstring & fname);
void _doSave(pwd::PwdMgr & pwdmgr, const std::wstring & fname);

//命令表
const CMDNode g_cmdMap[] = {
    CMDNode(L"help", help, L"help"),
    CMDNode(L"exit", 0, L"exit the app."),
    CMDNode(L"add", add, L"add"),
    CMDNode(L"view", view, L"view id | view /all"),
    CMDNode(L"del", del, L"del id"),
    CMDNode(L"modify", modify, L"modify id /a | modify id [/n][name][/p][pwd][/k][keyword][/d][desc]"),
    CMDNode(L"search", search, L"search /n name |search /k keword |search /d desc"),

    CMDNode(L"", 0, L""), //end mark
};

const CMDNode g_userCmdMap[] = {
    CMDNode(L"reload", userLoad, L"reload the data file."),
    CMDNode(L"save", userSave, L"save the data to file."),
    CMDNode(L"loadBackup", userLoadBackup, L"recover the data from backup file."),
    CMDNode(L"backup", userBackup, L"backup data to file."),

    CMDNode(L"", 0, L""), //end mark
};

const CMDNode g_adminCmdMap[] = {
    CMDNode(L"load", load, L"load filename"),
    CMDNode(L"save", save, L"save filename"),

    CMDNode(L"", 0, L""), //end mark
};

const std::wstring g_userDataFile = L"pwd/pwd.dat";
const std::wstring g_userBackupFile = L"pwd/pwd_backup.dat";

void doWork(pwd::PwdMgr & pwdmgr, const CMDNode *userCmdMap)
{
    const size_t MaxLen = 1024;
    wchar_t buffer[MaxLen];
    StringVector argv;

    while (true)
    {
        std::wcout << L">>";
        std::wcin.getline(buffer, MaxLen);
        parseCMDLine(argv, buffer);

        if (argv.empty()) continue;

        const std::wstring & cmd = argv[0];
        if (cmd == L"exit")
            break;

        //先在用户命令表中查找
        const CMDNode *pCMD = findCMD(userCmdMap, cmd);
        if (!pCMD)
        {
            //然后再全局表中查找
            pCMD = findCMD(g_cmdMap, cmd);
            if(!pCMD)
            {
                std::wcout << L"command not found! use 'help' to get more help." << std::endl;
                continue;
            }
        }

        if (!pCMD->fun_)
        {
            std::wcout << L"this command not implement." << std::endl;
            continue;
        }

        (*pCMD)(pwdmgr, argv, userCmdMap);
    }
}

//用户工作模式
void userMode()
{
    pwd::PwdMgr pwdmgr;
    std::wcout << L"password mgr version " << pwd::getVersion() << L"." << std::endl;
    std::wcout << L"user mode.\n" << std::endl;

    if (!_doLoad(pwdmgr, g_userDataFile))
    {
        std::wcout << L"can't load the pwd data, do you what continue?y/n" << std::endl;

        std::wstring str;
        while (true)
        {
            std::wcin >> str;
            if (str == L"y") break;
            else if (str == L"n") return;
        }
    }
    //自动备份
    _doSave(pwdmgr, g_userBackupFile);

    StringVector limits;
    limits.push_back(L"load");
    limits.push_back(L"save");

    doWork(pwdmgr, g_userCmdMap);
    _doSave(pwdmgr, g_userDataFile);
}

//管理员工作模式
void adminMode()
{
    pwd::PwdMgr pwdmgr;
    std::wcout << L"password mgr version " << pwd::getVersion() << L"." << std::endl;
    std::wcout << L"admin mode.\n" << std::endl;

    doWork(pwdmgr, g_adminCmdMap);
}

int _tmain(int argc, _TCHAR* argv[])
{
    resetcwd();

    if (argc > 1 && wcscmp(argv[1], L"/admin")==0)
    {
        adminMode();
        return 0;
    }
    
    userMode();
	return 0;
}

//command implement

DECLARE_CMDFUN(view)
{
    HOOK_HELP(view)

    if (argv.size() != 2)
    {
        std::wcout << L"invalid arguments. input an id or '/all'" << std::endl;
        return;
    }

    if (argv[1] == L"/all" || argv[1] == L"/a")
    {
        for (pwd::PwdMap::const_iterator it = pwdmgr.begin();
            it != pwdmgr.end(); ++it)
        {
            _view(it->second);
        }
    }
    else
    {
        long id = _wtol(argv[1].c_str());

        try
        {
            const pwd::Pwd & data = pwdmgr.get(id);
            _view(data);
        }
        catch (std::exception e)
        {
            std::wcout << e.what() << std::endl;
        }
    }
}

DECLARE_CMDFUN(help)
{
    const CMDNode * pNode = g_cmdMap;
    while (!pNode->cmd_.empty())
    {
        std::wcout << pNode->cmd_ << " : " << pNode->desc_ << std::endl;

        ++pNode;
    }

    pNode = userCmdMap;
    while (!pNode->cmd_.empty())
    {
        std::wcout << pNode->cmd_ << " : " << pNode->desc_ << std::endl;

        ++pNode;
    }
}

DECLARE_CMDFUN(load)
{
    HOOK_HELP(load)

    if (argv.size() != 2)
    {
        std::wcout << L"need a file name." << std::endl;
        return;
    }

    _doLoad(pwdmgr, argv[1]);
}

DECLARE_CMDFUN(save)
{
    HOOK_HELP(save)

    if (argv.size() != 2)
    {
        std::wcout << L"need a file name." << std::endl;
        return;
    }
    _doSave(pwdmgr, argv[1]);
}

DECLARE_CMDFUN(add)
{
    if (argv.size() != 1)
    {
        std::wcout << L"invalid arguments." << std::endl;
        return;
    }

    pwd::Pwd data;
    _add(data);
    pwd::pwdid id = pwdmgr.add(data);
    std::wcout << L"succed insert the data. id=" << id << std::endl;
}

DECLARE_CMDFUN(del)
{
    HOOK_HELP(del)

    if (argv.size() != 2)
    {
        std::wcout << L"invalid arguments. input an id." << std::endl;
        return;
    }

    long id = _wtol(argv[1].c_str());

    try
    {
        const pwd::Pwd & data = pwdmgr.get(id);
        _view(data);
    }
    catch (std::exception e)
    {
        std::wcout << L"del failed!" << e.what() << std::endl;
        return;
    }

    if(ensure(L"ensure to del?"))
    {
        pwdmgr.del(id);
    }
}

DECLARE_CMDFUN(modify)
{
    HOOK_HELP(modify)

    if (argv.size() < 3)
    {
        std::wcout << L"need at least 2 arguments." << std::endl;
        return;
    }

    long id = _wtol(argv[1].c_str());

    pwd::Pwd data;
    try
    {
        data = pwdmgr.get(id);
    }
    catch (std::exception e)
    {
        std::wcout << e.what() << std::endl;
        return;
    }

    if (argv[2] == L"/a" || argv[1] == L"/all")
    {
        _add(data);
    }
    else
    {
        for (size_t i = 2; i < argv.size() - 1; ++i)
        {
            const std::wstring & arg = argv[i];
            if (arg == L"/n" || arg == L"/name")
            {
                data.name_ = argv[i + 1];
            }
            else if (arg == L"/p" || arg == L"/pwd")
            {
                data.pwd_ = argv[i + 1];
            }
            else if (arg == L"/k" || arg == L"/keyword")
            {
                data.keyword_ = argv[i + 1];
            }
            else if (arg == L"/d" || arg == L"desc")
            {
                data.desc_ = argv[i + 1];
            }
            else
            {
                continue;
            }

            ++i;//skip the value
        }
    }

    pwdmgr.modify(id, data);
    
}

DECLARE_CMDFUN(search)
{
    HOOK_HELP(search)

    if (argv.size() < 2)
    {
        std::wcout << L"need at least 1 arguments." << std::endl;
        return;
    }

    std::wstring key, value;

    if (argv.size() == 2)
    {
        key = _T("/k");
        value = argv[1];
    }
    else
    {
        key = argv[1];
        value = argv[2];
    }

    pwd::IdVector ids;

    if (key == L"/n" || key == L"/name")
    {
        pwdmgr.searchByName(ids, value);
    }
    else if (key == L"/k" || key == L"/keyword")
    {
        pwdmgr.searchByKeyword(ids, value);
    }
    else if (key == L"/d" || key == L"desc")
    {
        pwdmgr.searchByDesc(ids, value);
    }
    else
    {
        std::wcout << L"invalid argument." << std::endl;
        return;
    }

    if (ids.empty())
    {
        std::wcout << L"the data was not found." << std::endl;
        return;
    }

    std::wcout << L"find ids:" << std::endl;
    for (size_t i = 0; i < ids.size(); ++i)
    {
        std::wcout << ids[i] << std::endl;
    }

    for (size_t i = 0; i < ids.size(); ++i)
    {
        _view(pwdmgr.get(ids[i]));
    }
}

DECLARE_CMDFUN(userLoad)
{
    if(ensure(L"current data will lost! ensure to reload?"))
    {
        _doLoad(pwdmgr, g_userDataFile);
    }
}

DECLARE_CMDFUN(userSave)
{
    _doSave(pwdmgr, g_userDataFile);
}

DECLARE_CMDFUN(userLoadBackup)
{
    _doLoad(pwdmgr, g_userBackupFile);
}

DECLARE_CMDFUN(userBackup)
{
     _doSave(pwdmgr, g_userBackupFile);
}

//helper method

void _view(const pwd::Pwd & data)
{
    std::wcout << std::endl;
    std::wcout << L"id:     " << data.id_ << std::endl;
    std::wcout << L"---------------------------------------------------------" << std::endl;
    std::wcout << L"name:   " << data.name_ << std::endl;
    std::wcout << L"pwd:    " << data.pwd_ << std::endl;
    std::wcout << L"keyword:" << data.keyword_ << std::endl;
    std::wcout << L"desc:   " << data.desc_ << std::endl;
    std::wcout << L"---------------------------------------------------------" << std::endl;
}

void _add(pwd::Pwd & data)
{
    const size_t MaxLen = 1024;
    wchar_t buffer[MaxLen];

    std::wcout << L"name:";

    do
    {
        std::wcin.getline(buffer, MaxLen);
    }while(buffer[0] == 0);

    data.name_ = buffer;

    std::wcout << L"pwd:";
    std::wcin.getline(buffer, MaxLen);
    data.pwd_ = buffer;

    std::wcout << L"keyword:";
    std::wcin.getline(buffer, MaxLen);
    data.keyword_ = buffer;

    std::wcout << L"desc:";
    std::wcin.getline(buffer, MaxLen);
    data.desc_ = buffer;
}

bool _doLoad(pwd::PwdMgr & pwdmgr, const std::wstring & fname)
{
    FILE* pFile = openFile(fname, L"rb");
    if (!pFile)
    {
        std::wcout << L"open file " << fname << " failed!" << std::endl;
        return false;
    }

    pwd::PwdStream stream;
    pwd::streambuffer & buffer = stream.steam();

    long len = flength(pFile);
    buffer.resize(len);
    if (fread(&buffer[0], len, 1, pFile) != 1)
    {
        std::wcout << L"read file failed!" << std::endl;
        fclose(pFile);
        return false;
    }
    fclose(pFile);

    if (!pwdmgr.load(stream))
    {
        std::wcout << L"load stream failed!" << std::endl;

        return false;
    }

    std::wcout << L"succed load pwd data '" << fname << "'." << std::endl;
    return true;
}

void _doSave(pwd::PwdMgr & pwdmgr, const std::wstring & fname)
{
    FILE* pFile = openFile(fname, L"wb");
    if (!pFile)
    {
        std::wcout << L"open file " << fname << " failed!" << std::endl;
        return;
    }

    pwd::PwdStream stream;
    if (!pwdmgr.save(stream))
    {
        std::wcout << L"save stream failed!" << std::endl;
        fclose(pFile);
        return;
    }

    pwd::streambuffer & buffer = stream.steam();
    size_t len = buffer.size();

    if (fwrite(&buffer[0], len, 1, pFile) != 1)
    {
        std::wcout << L"write file failed!" << std::endl;
        fclose(pFile);
        return;
    }
    fclose(pFile);

    std::wcout << L"succed save pwd data '" << fname << "'." << std::endl;
}
