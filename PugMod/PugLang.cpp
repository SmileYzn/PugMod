#include "precompiled.h"

CPugLang gPugLang;

void CPugLang::ServerActivate()
{
    this->m_Data.clear();

    char File[MAX_PATH] = { 0 };
    snprintf(File, sizeof(File), "%s/cfg/language.cfg", gPugUtil.GetFullPath());

    std::ifstream TempFile(File);

    if (TempFile.is_open())
    {
        std::string Line;
        std::string Head;
        std::string Lang;

        while (std::getline(TempFile, Line))
        {
            if (!Line.empty())
            {
                if (Line.find('"') == 0)
                {
                    Line.erase(std::remove(Line.begin(), Line.end(), '\"'), Line.end());
                    Head = Line;
                }
                if (Line.find(':') == 2)
                {
                    Lang = Line.substr(0, 2);

                    if (!Lang.empty() && !Line.empty())
                    {
                        Line.erase(0, 3);
                        Line.erase(std::remove(Line.begin(), Line.end(), '\"'), Line.end());

                        if (!Head.empty() && !Line.empty())
                        {
                            this->m_Data[Head][Lang] = Line;
                        }
                    }
                }
            } 
        }
    }

    TempFile.close();
}

const char* CPugLang::Get(const char* Head)
{
    if (Head)
    {
        if (Head[0u] != '\0')
        {
            if (gPugCvar.m_Language->string)
            {
                if (gPugCvar.m_Language->string[0u] != '\0')
                {
                    if (!this->m_Data.empty())
                    {
                        if (this->m_Data.find(Head) != this->m_Data.end())
                        {
                            if (this->m_Data[Head].find(gPugCvar.m_Language->string) != this->m_Data[Head].end())
                            {
                                return this->m_Data[Head][gPugCvar.m_Language->string].c_str();
                            }
                        }
                    }
                }
            }
        }
    }

    return Head;
}