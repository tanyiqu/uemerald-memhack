/**
 * 导出相关 名字/技能名字等TXT文档
 */
#include "util.h"
int main()
{
    map<u16,string> codeMap;
    loadCodeMap(codeMap);
    getSkillName(codeMap);
    getBreedName(codeMap);
    getItemName(codeMap);
    return 0;
}