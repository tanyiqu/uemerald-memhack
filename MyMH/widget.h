#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTableWidgetItem>
#include <QDir>
#include "visualboyadvance.h"
#include "pokemon.h"

#define BREEDNUM 1201
#define ITEMNUM 801
#define BALLNUM 27
#define SKILLNUM 801//676
#define AREANUM 256
#define UNCODENUM 6824

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);

    ~Widget();

    bool detect();                  //检测软件是否可用
    void dialog();                  //开始对话框

    QString get_cpuId();

    void CalPokemonOffset();        //计算其余5只PM的偏移
    void CalExpList();
    void loadType();
    void loadAbility();
    void uncodef();
    void loadCodeMap();
    QString decode(u8 *data,int size);
    void readFile();                //从文件中读取种族,物品
    void addItem();                 //添加comboBox选项
    //宠物
    void readPoke(int index);       //读取PM信息
    void showExpLevel();            //等级
    void showEnd();                 //读取努力值
    void showId();                  //个体值
    void showHideID();              //ID
    void showBeauty();              //魅力值
    void showAbValue();             //能力值
    void showTrim();                //强迫症
    void showMark();                //标记
    void showSex();                 //性别
    void showColor();               //闪光
    void showSkill_1();             //前两个技能
    void showSkill_2();             //后两个技能
    void showEgg();                 //蛋
    void showEggSteps();            //孵蛋圈数
    void showHide();                //隐藏特性
    void showAbility();             //显示特性
    void showPP();                  //PP
    void showPointUp();             //PP增幅
    void showRibbon();              //缎带
    void showPokeName();            //名字
    void showOTName();              //主人名字
    void showStatus();              //状态
    void showFixed();               //固定属性
    void showBreedValue();          //种族值
    void showMegaLock();            //锁定了mega状态
    void writeEnd();                //写努力值
    void writeId();                 //个体值
    void writeHideID();             //ID
    void writeBeauty();             //魅力值
    void writeAbValue();            //能力值
    void writeTrim();               //强迫症
    void writeMark();               //标记
    void writeAbility();            //特性
    void writePP();                 //PP
    void writePointUp();            //PP增幅
    void writeRibbon();             //缎带
    void writeStatus();             //状态
    void writePersValue();          //性格
    void refreshSkill();
    //角色
    void showPlayerName();
    void showPlayerID();
    void showMoney();
    void showCoin();
    void showDust();
    void showRestSteps();
    void showBattlePoint();
    void showBattlePointOnCard();

    void writePlayerName();
    void writePlayerID();
    void writeMoney();
    void writeCoin();
    void writeDust();
    void writeRestSteps();
    void writeBattlePoint();
    void writeBattlePointOnCard();

private slots:
    void on_refresh_clicked();      //主界面刷新按钮
    void on_write_clicked();
    void on_refreshPlayer_clicked();//角色刷新按钮
    void on_writePlayer_clicked();
    void on_refreshItem_clicked();  //背包刷新按钮
    void on_writeItem_clicked();
    void on_battleRefresh_s_clicked();//单打刷新按钮
    void on_battleWrite_s_clicked();
    void on_battleRefresh_d_clicked();//双打刷新按钮
    void on_battleWrite_d_clicked();

    void on_poke1_clicked();
    void on_poke2_clicked();
    void on_poke3_clicked();
    void on_poke4_clicked();
    void on_poke5_clicked();
    void on_poke6_clicked();
    void on_searchBreed_clicked();
    void on_searchItem_clicked();
    void on_searchPlayerItem_clicked();
    void on_searchSkill1_clicked();
    void on_searchSkill2_clicked();
    void on_searchSkill3_clicked();
    void on_searchSkill4_clicked();
    void on_changeColor_clicked();
    void on_comboLevel_currentIndexChanged(int index);
    void on_tab_pokeinfo_currentChanged(int index);
    void on_changeSkill_clicked();
    void on_LeagueMode_clicked();
    void on_MaxPointUp_clicked();
    void on_MaxBeauty_clicked();
    void on_MaxID_clicked();
    void on_toEgg_clicked();
    void on_changeId_clicked();
    void on_changeHideId_clicked();
    void on_ribbon20_clicked();
    void on_ribbon25_clicked();
    void on_ribbon32_clicked();
    void on_ribbon0_clicked();
    void on_copy_clicked();
    void on_paste_clicked();
    void on_Tip_clicked();
    void on_comboPers_currentIndexChanged(int index);
    void on_comboSex_currentIndexChanged(int index);
    void on_switch_snipaste_clicked();
    void on_stepsTo0_clicked();
    void on_deletePokemon_clicked();
    void on_toTieba_clicked();
    void on_enableSettings_clicked();
    void on_correction_clicked();
    void on_tabWidget_currentChanged(int index);
    void on_lockHP_s_clicked();
    void on_searchName_clicked();
    void on_lineNameInput_returnPressed();
    void on_lineNameInput_textChanged(const QString &text);
    void on_nameInput_textChanged(const QString &text);
    void on_lineOTNameInput_textChanged(const QString &text);
    void on_searchPokeName_clicked();
    void on_searchOTName_clicked();
    void on_nameInput_returnPressed();
    void on_lineOTNameInput_returnPressed();
    void on_writePokeName_clicked();
    void on_writeOTName_clicked();
    void on_oneHitKill_s_clicked();
    void on_battleDetect_s_clicked();
    void on_invincible_s_clicked();
    void on_onekeyCloseLocking_s_clicked();
    void on_getVBA_clicked();    
    void on_battleDetect_d_clicked();
    void on_onekeyCloseLocking_d_clicked();
    void on_lockHP_d_clicked();
    void on_oneHitKill_d_clicked();
    void on_invincible_d_clicked();
    void on_lockMega_clicked();
    void on_itemTable_itemClicked(QTableWidgetItem *item);
    void on_deleteItem_clicked();

    void on_EnableMirageIsland_clicked();

    void on_getMCode_clicked();


    void on_normalItemTo99_clicked();

    void on_getMegaVersion_clicked();

    void on_allSkillItem_clicked();

    void on_fruitTo99_clicked();

    void on_ballTo99_clicked();

    void on_howToUse_clicked();

    void on_repository_clicked();

    void on_homePage_clicked();

protected:
    void keyPressEvent(QKeyEvent *);

private:
    Ui::Widget *ui;
    VisualBoyAdvance VBA;           //VBA 提供读写功能
    Pokemon poke[7];                //第一个位置放弃
    CurrPokeMon currentPoke;        //指定当前选择的PM
    BreedList breed[BREEDNUM];      //种族列表
    ItemList item[ITEMNUM];         //物品列表
    SkillList skill[SKILLNUM];      //技能列表
    BallList ball[BALLNUM];         //精灵球列表
    AreaList area[AREANUM];         //地区列表
    Uncode uncode[UNCODENUM];       //
    ExpType type[6];                //经验值列表
    QStringList Personality;        //性格列表
    PokeType pokeType[24];          //属性列表
    QString ability[256];           //特性列表
    BYTE snipaste[100] = {213,1,213,1,0,0,0,0,9,102,5,80,255,100,0,3,112,21,2,2,14,44,14,234,255,255,255,0,0,0,0,0,151,0,1,0,20,44,16,0,255,255,0,0,19,0,127,0,57,0,15,0,24,24,24,48,0,252,0,252,0,6,0,0,0,0,0,0,15,201,30,235,255,255,255,63,36,201,255,135,0,0,0,0,100,255,85,1,85,1,72,1,236,0,43,1,212,0,237,0};
    int useLock;
    QString lockFileDirPathName = tr("C://ProgramData/MyMH/userdata");
    QString lockFilePathName = tr("C://ProgramData/MyMH/userdata/usr.mdat");
    QString lockFileWarningName = tr("C://ProgramData/MyMH/userdata/别瞎改了,加密过的!!.txt");
    QDir lockFileDir;
    QFile lockFile;
    QFile lockFileWarning;
    u16 lockedHP;
    QTimer *timer_lockHp_s = NULL;
    QTimer *timer_oneHit_s = NULL;
    QTimer *timer_BattDected_s = NULL;
    QTimer *timer_invincible_s = NULL;
    u16 lockedHP2;
    QTimer *timer_lockHp_d = NULL;
    QTimer *timer_oneHit_d = NULL;
    QTimer *timer_BattDected_d = NULL;
    QTimer *timer_invincible_d = NULL;
    QTimer *timer_lockMega = NULL;
    QString key;
    bool isActivate = false;
    bool createdLockFile = false;
    bool addedFlag = false;
    bool addedPlayerItem = false;
    bool readedFile = false;
    bool readedSkill = false;
    bool addedSkill = false;
    bool uncoded = false;
    QMap<u16,QString> codeMap;
//    map<u16,string>& codeMap;
};

#endif // WIDGET_H
