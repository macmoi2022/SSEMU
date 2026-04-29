// BadSyntax.cpp: implementation of the CBadSyntax class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BadSyntax.h"
#include "ReadFile.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBadSyntax::CBadSyntax() // OK
{
	this->m_BadSyntaxInfo.clear();
}

CBadSyntax::~CBadSyntax() // OK
{

}

void CBadSyntax::Load(char* path) // OK
{
	CReadFile* lpReadScript = new CReadFile;

	if(lpReadScript == 0)
	{
		ErrorMessageBox(READ_FILE_ALLOC_ERROR,path);
		return;
	}

	if(lpReadScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpReadScript->GetLastError());
		delete lpReadScript;
		return;
	}

	this->m_BadSyntaxInfo.clear();

	try
	{
		while(true)
		{
			if(lpReadScript->GetToken() == TOKEN_END)
			{
				break;
			}

			if(strcmp("end", lpReadScript->GetString()) == 0)
			{
				break;
			}

			BAD_SYNTAX_INFO info;

			strcpy_s(info.syntax, lpReadScript->GetString());

			this->m_BadSyntaxInfo.push_back(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpReadScript->GetLastError());
	}

	delete lpReadScript;
}

bool CBadSyntax::CheckSyntax(char* text) // OK
{
	for(std::vector<BAD_SYNTAX_INFO>::iterator it=this->m_BadSyntaxInfo.begin();it != this->m_BadSyntaxInfo.end();it++)
	{
		if(strstr(text,it->syntax) != 0)
		{
			return 0;
		}
	}

	return 1;
}
