#pragma once
#ifndef POKEMON_H
#define POKEMON_H
#include <windows.h>
#include <QString>
#include <QPixmap>

typedef unsigned int u32;
typedef unsigned short int u16;
typedef unsigned char u8;

struct Pokemon
{
    bool megaLocked = false;
    u16 megaLockedNum;
    int breedOffset	= 0x0002450C;		//2byte
    int ExpOffset	= 0x00024510;		//4byte
    int levelOffset	= 0x00024540;		//1byte
    int PersOffset	= 0x000244EC;		//4byte
    int itemOffset	= 0x0002450E;		//2byte
    int abiliOffset	= 0x00024534;		//4byte
    int intiOffset	= 0x00024515;		//1byte
    int end_hp		= 0x00024524;		//1byte
    int end_atk		= 0x00024525;		//1byte
    int end_def		= 0x00024526;		//1byte
    int end_sp		= 0x00024527;		//1byte
    int end_spatk	= 0x00024528;		//1byte
    int end_spdef	= 0x00024529;		//1byte
    int virusOffset	= 0x00024530;		//1byte
    int totalHP		= 0x00024544;		//2byte
    int Atk			= 0x00024546;		//2byte
    int Def			= 0x00024548;		//2byte
    int Speed		= 0x0002454A;		//2byte
    int SpAtk		= 0x0002454C;		//2byte
    int SpDef		= 0x0002454E;		//2byte
    int markOffset	= 0x00024507;		//1byte
    int beauty1		= 0x0002452A;		//1byte
    int beauty2		= 0x0002452B;		//1byte
    int beauty3		= 0x0002452C;		//1byte
    int beauty4		= 0x0002452D;		//1byte
    int beauty5		= 0x0002452E;		//1byte
    int beauty6		= 0x0002452F;		//1byte
    int ball        = 0x00024533;		//1byte
    int PP1			= 0x00024520;		//1byte
    int PP2			= 0x00024521;		//1byte
    int PP3			= 0x00024522;		//1byte
    int PP4			= 0x00024523;		//1byte
    int PointUp		= 0x00024514;		//1byte
    int ID			= 0x000244F0;		//4byte
    int skill_1		= 0x00024518;		//2byte
    int skill_2		= 0x0002451A;		//2byte
    int skill_3		= 0x0002451C;		//2byte
    int skill_4		= 0x0002451E;		//2byte
    int HideID		= 0x0002450A;		//2byte
    int Ribbon      = 0x00024538;       //4byte
    int Status      = 0x0002453C;       //1byte
    int begin       = 0x000244EC;       //100byte

};

struct ROM
{
    u32 sex = 0x00F186E0 + 16;
    u32 b_hp = 0x00F186E0 + 0;
    u32 b_atk = 0x00F186E0 + 1;
    u32 b_def = 0x00F186E0 + 2;
    u32 b_sp = 0x00F186E0 + 3;
    u32 b_spatk = 0x00F186E0 + 4;
    u32 b_spdef = 0x00F186E0 + 5;
};

struct PokeType
{
    QPixmap pix;
};

struct CurrPokeMon
{
    int index;
    u16 breed;
    u32 pers;
    u8 name[10];
    u8 OTName[7];
    u32 ability;
    u16 Item;
    u8 virus;
    u8 inti;
    u32 exp;
    u8 level;
    u16 totalHp;//实际能力
    u16 Atk;
    u16 Def;
    u16 Speed;
    u16 SpAtk;
    u16 SpDef;
    u8 end_hp;//努力值
    u8 end_atk;
    u8 end_def;
    u8 end_sp;
    u8 end_spatk;
    u8 end_spdef;
    int IVs_hp;//个体值
    int IVs_atk;
    int IVs_def;
    int IVs_sp;
    int IVs_spatk;
    int IVs_spdef;
    u8 b1;//魅力值
    u8 b2;
    u8 b3;
    u8 b4;
    u8 b5;
    u8 b6;
    u8 mark;
    u8 ball;
    u8 PP1;
    u8 PP2;
    u8 PP3;
    u8 PP4;
    u8 PointUp;
    u16 skill_1;
    u16 skill_2;
    u16 skill_3;
    u16 skill_4;
    u8 sex;     //0公 1母 2无性别
    u8 sexRatio;
    u8 v[6];    //种族值
    u16 hideID;
    u8 expType;
    u8 status;
};

struct BreedList
{
    int num;
    QString name;
};

struct ItemList
{
    int num;
    QString name;
};

struct BallList
{
    QString name;
    u8 value;
};

struct SkillList
{
    int num;
    QString name;
};

struct AreaList
{
    int num;
    QString name;
};

struct ExpType
{
    int expList[101];
};

struct Uncode
{
    QChar Char;
    QString uni;
};
#endif // POKEMON_H
