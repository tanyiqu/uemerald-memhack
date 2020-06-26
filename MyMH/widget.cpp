#include "widget.h"
#include "ui_widget.h"
#include "visualboyadvance.h"
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QTime>
#include <QTimer>
#include <QPainter>
#include <Qdir>
#include <QKeyEvent>
#include <QUrl>
#include <QDesktopServices>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QProcess>
#include <QClipboard>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    setStyleSheet("font-size:12px");
    currentPoke.index = 1;
    setFixedSize(this->size());
    CalPokemonOffset();
    CalExpList();
    loadType();
    loadAbility();
    ui->refresh->setFocus();
    loadCodeMap();
//    ui->comboVersion->setCurrentIndex(1);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::keyPressEvent(QKeyEvent *e)//键盘事件
{
    if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
    {
        if(ui->comboBreed->hasFocus())
        {
            qDebug() << "BreedCombo";
            on_searchBreed_clicked();
        }
        else if(ui->comboItem->hasFocus())
        {
            qDebug() << "ItemCombo";
            on_searchItem_clicked();
        }
        else if(ui->comboPlayerItem->hasFocus())
        {
            qDebug() << "PlayerItemCombo";
            on_searchPlayerItem_clicked();
        }
        else if(ui->skill1->hasFocus())
        {
            qDebug() << "Skill1";
            on_searchSkill1_clicked();
        }
        else if(ui->skill2->hasFocus())
        {
            qDebug() << "Skill2";
            on_searchSkill2_clicked();
        }
        else if(ui->skill3->hasFocus())
        {
            qDebug() << "Skill3";
            on_searchSkill3_clicked();
        }
        else if(ui->skill4->hasFocus())
        {
            qDebug() << "Skill4";
            on_searchSkill4_clicked();
        }
    }
}

bool Widget::detect()
{
    lockFileDir.setPath(lockFileDirPathName);
    lockFile.setFileName(lockFilePathName);
    lockFileWarning.setFileName(lockFileWarningName);
    //检测电脑有没有lock文件夹
    if(!lockFileDir.exists())
    {
        qDebug() << "目录不存在";
        bool res = lockFileDir.mkpath(lockFileDirPathName);
        if(res)
            qDebug() << "新建目录成功";
        else
            qDebug() << "失败";
    }
    //检测电脑有没有lock文件
    if(!lockFile.exists())
    {
        qDebug() << "文件不存在";
        lockFile.open(QIODevice::ReadWrite | QIODevice::Text);
        lockFile.write("0\n");//是否显示开始对话框
        lockFile.write("3\n");//试用次数
        lockFile.write(QString(get_cpuId() + "\n").toUtf8().data());//本机机器码
        lockFile.write(get_cpuId().toUtf8().data());//激活码
        lockFile.close();
        qDebug() << "文件创建成功";
    }
    if(!lockFileWarning.exists())
    {
        lockFileWarning.open(QIODevice::ReadWrite | QIODevice::Text);
        lockFileWarning.write(tr("").toUtf8().data());
        lockFileWarning.close();
    }

    //读取文件
    lockFile.open(QIODevice::ReadOnly);
    QString startDialog(lockFile.readLine());startDialog.chop(2);   //开始对话框
    int tryNum = QString(lockFile.readLine()).toInt();              //试用次数
    QString cpuid(lockFile.readLine());cpuid.chop(2);               //机器码
    this->key = QString(lockFile.readLine());                     //激活码
    lockFile.close();
//    qDebug() << startDialog << cpuid << this->key;

    //--------------------------------------------
    cpuid = get_cpuId();
    QString cpuid1 = cpuid.mid(0,8);
    QString cpuid2 = cpuid.mid(8,8);
    u32 c1 = cpuid1.toUInt(nullptr,16);
    u32 c2 = cpuid2.toUInt(nullptr,16);

    QString key1 = this->key.mid(0,8);
    QString key2 = this->key.mid(8,8);
    u32 k1 = key1.toUInt(nullptr,16);
    u32 k2 = key2.toUInt(nullptr,16);
    if((c1^k1)==0xABCD9876 && (c2^k2)==0x9876ABCD)
        isActivate = true;
    else
        isActivate = false;
    //是否激活
    if(!isActivate)
    {
        QDialog dlg(this);
        dlg.setFixedSize(260,102);
        dlg.setWindowTitle(tr("试用%1次").arg(tryNum));
        dlg.setWindowFlags(Qt::Dialog | Qt::WindowMinimizeButtonHint);
        QLabel text;
        text.setStyleSheet(""
                        "font: 10pt \"微软雅黑\";"
                        "font-size:12px;"
                        "");
        text.setGeometry(58,14,170,50);
        text.setText(tr("本机机器码：\n激活后此对话框自动消失\n激活码："));
        text.setParent(&dlg);
        QLineEdit *edit1 = new QLineEdit(&dlg);
        edit1->setGeometry(130,14,120,18);
        edit1->setReadOnly(true);
        edit1->setText(get_cpuId());
        QLineEdit *edit2 = new QLineEdit(&dlg);
        edit2->setGeometry(130,48,120,18);
        QLabel icon;
        icon.setGeometry(10,14,40,40);
        icon.setPixmap(QPixmap("://images/icon.png"));
        icon.setParent(&dlg);
        QPushButton b(tr("试用"));
        QPushButton z(tr("激活"));
        QPushButton a(tr("获取激活码"));
        b.setGeometry(15,70,75,25);
        z.setGeometry(95,70,75,25);
        a.setGeometry(175,70,75,25);
        b.setParent(&dlg);
        z.setParent(&dlg);
        a.setParent(&dlg);
        connect(&b,&QPushButton::clicked,&dlg,&QDialog::close);
        connect(&z,&QPushButton::clicked,[=](){
            QString c1 = edit1->text().mid(0,8);
            QString c2 = edit1->text().mid(8,8);
            QString k1 = edit2->text().mid(0,8);
            QString k2 = edit2->text().mid(8,8);
            u32 uc1 = c1.toUInt(nullptr,16);
            u32 uc2 = c2.toUInt(nullptr,16);
            u32 uk1 = k1.toUInt(nullptr,16);
            u32 uk2 = k2.toUInt(nullptr,16);
            if((uc1^uk1)==0xABCD9876 && (uc2^uk2)==0x9876ABCD)
            {
                this->key = edit2->text();
                QMessageBox::warning(this,tr("激活成功"),tr("激活成功\n重启后生效"));
                lockFile.open(QIODevice::WriteOnly);
                lockFile.write(QString(startDialog + "\r\n").toUtf8().data());
                lockFile.write(QString("%1\r\n").arg(tryNum).toUtf8().data());
                lockFile.write(QString(cpuid + "\r\n").toUtf8().data());
                lockFile.write(this->key.toUtf8().data());
                lockFile.close();
                exit(0);
            }
            else
                QMessageBox::warning(this,tr("激活失败"),tr("激活失败\n请联系淘宝卖家获取激活码！"));
        });
        connect(&a,&QPushButton::clicked,[=](){
            const QUrl regUrl(QLatin1String("https://shop392021599.taobao.com"));
            QDesktopServices::openUrl(regUrl);
        });
        dlg.exec();
//        lockFile.open(QIODevice::WriteOnly);
//        lockFile.write(QString(startDialog + "\n").toUtf8().data());
//        lockFile.write(QString("%1\n").arg(tryNum).toUtf8().data());
//        lockFile.write(QString(cpuid + "\n").toUtf8().data());
//        lockFile.write(this->key.toUtf8().data());
//        lockFile.close();
        if(tryNum <= 0)
            exit(0);
        //------------------修改了使用次数
//        if(tryNum >3){QMessageBox::critical(this,"MyPokeHack",tr("修改了使用次数？？\n好像没有用呢！"),QMessageBox::Ok);exit(0); }
        //------------------
        if(tryNum > 0)
            tryNum --;
        lockFile.open(QIODevice::WriteOnly);
        lockFile.write(QString(startDialog + "\r\n").toUtf8().data());
        lockFile.write(QString("%1\r\n").arg(tryNum).toUtf8().data());
        lockFile.write(QString(cpuid + "\r\n").toUtf8().data());
        lockFile.write(this->key.toUtf8().data());
        lockFile.close();
    }
    //----------------------------------------------------

    //是否显示开始对话框
    if(startDialog == "0")//显示对话框
    {
        ui->noStartDialog->setChecked(false);
        dialog();
    }
    else
        ui->noStartDialog->setChecked(true);
    showSkill_1();
    return true;
}

void Widget::dialog()
{
    QDialog dlg(this);
    dlg.setFixedSize(220,102);
    dlg.setWindowTitle(tr("公告"));
    dlg.setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    QLabel text;
    text.setStyleSheet(""
                    "font: 10pt \"微软雅黑\";"
                    "font-size:12px;"
                    "");
//    text.setGeometry(58,17,150,34);
//    text.setText(tr("本软件完全免费,禁止倒卖!"));
    text.setGeometry(58,7,150,50);
    text.setText(tr("本软件禁止倒卖!\n可以的话，\n还是请多少赞助点！嘿嘿！"));
    text.setParent(&dlg);
    QLabel icon;
    icon.setGeometry(10,14,40,40);
    icon.setPixmap(QPixmap("://images/icon.png"));
    icon.setParent(&dlg);
    QPushButton b(tr("确认"));
    b.setGeometry(130,60,75,25);
    b.setParent(&dlg);
    connect(&b,&QPushButton::clicked,&dlg,&QDialog::close);
    dlg.exec();
}

void Widget::CalPokemonOffset()
{
    Personality << tr("努力 无变化") << tr("孤僻 物攻+ 物防-") << tr("勇敢 物攻+ 速度-") << tr("固执 物攻+ 特攻-") <<
    tr("调皮 物攻+ 特防-") << tr("大胆 物防+ 物攻-") << tr("坦率 无变化") << tr("悠闲 物防+ 速度-") <<
    tr("淘气 物防+ 特攻-") << tr("无虑 物防+ 特防-") << tr("胆小 速度+ 物攻-") << tr("急躁 速度+ 物防-") <<
    tr("认真 无变化") << tr("开朗 速度+ 特攻-") << tr("天真 速度+ 特防-") << tr("保守 特攻+ 物攻-") <<
    tr("稳重 特攻+ 物防-") << tr("冷静 特攻+ 速度-") << tr("害羞 无变化") << tr("马虎 特攻+ 特防-") <<
    tr("沉着 特防+ 物攻-") << tr("温顺 特防+ 物防-") << tr("狂妄 特防+ 速度-") << tr("慎重 特防+ 特攻-") << tr("浮躁 无变化");
    for (int i = 2; i <= 6; i++)
    {
        poke[i].breedOffset = poke[i - 1].breedOffset + 100;
        poke[i].ExpOffset = poke[i - 1].ExpOffset + 100;
        poke[i].levelOffset = poke[i - 1].levelOffset + 100;
        poke[i].PersOffset = poke[i - 1].PersOffset + 100;
        poke[i].itemOffset = poke[i - 1].itemOffset + 100;
        poke[i].abiliOffset = poke[i - 1].abiliOffset + 100;
        poke[i].intiOffset = poke[i - 1].intiOffset + 100;
        poke[i].end_hp = poke[i - 1].end_hp + 100;
        poke[i].end_atk = poke[i - 1].end_atk + 100;
        poke[i].end_def = poke[i - 1].end_def + 100;
        poke[i].end_sp = poke[i - 1].end_sp + 100;
        poke[i].end_spatk = poke[i - 1].end_spatk + 100;
        poke[i].end_spdef = poke[i - 1].end_spdef + 100;
        poke[i].virusOffset = poke[i - 1].virusOffset + 100;
        poke[i].totalHP = poke[i - 1].totalHP + 100;
        poke[i].Atk = poke[i - 1].Atk + 100;
        poke[i].Def = poke[i - 1].Def + 100;
        poke[i].Speed = poke[i - 1].Speed + 100;
        poke[i].SpAtk = poke[i - 1].SpAtk + 100;
        poke[i].SpDef = poke[i - 1].SpDef + 100;
        poke[i].markOffset = poke[i - 1].markOffset + 100;
        poke[i].beauty1 = poke[i - 1].beauty1 + 100;
        poke[i].beauty2 = poke[i - 1].beauty2 + 100;
        poke[i].beauty3 = poke[i - 1].beauty3 + 100;
        poke[i].beauty4 = poke[i - 1].beauty4 + 100;
        poke[i].beauty5 = poke[i - 1].beauty5 + 100;
        poke[i].beauty6 = poke[i - 1].beauty6 + 100;
        poke[i].ball = poke[i - 1].ball + 100;
        poke[i].PP1 = poke[i - 1].PP1 + 100;
        poke[i].PP2 = poke[i - 1].PP2 + 100;
        poke[i].PP3 = poke[i - 1].PP3 + 100;
        poke[i].PP4 = poke[i - 1].PP4 + 100;
        poke[i].PointUp = poke[i - 1].PointUp + 100;
        poke[i].ID = poke[i - 1].ID + 100;
        poke[i].skill_1 = poke[i - 1].skill_1 + 100;
        poke[i].skill_2 = poke[i - 1].skill_2 + 100;
        poke[i].skill_3 = poke[i - 1].skill_3 + 100;
        poke[i].skill_4 = poke[i - 1].skill_4 + 100;
        poke[i].HideID = poke[i - 1].HideID + 100;
        poke[i].Ribbon = poke[i - 1].Ribbon + 100;
        poke[i].Status = poke[i - 1].Status + 100;
        poke[i].begin = poke[i - 1].begin + 100;
    }
}
void Widget::CalExpList()
{
    type[0].expList[0] = 0 ;type[1].expList[0] = 0 ;type[2].expList[0] = 0 ;type[3].expList[0] = 0 ;type[4].expList[0] = 0 ;type[5].expList[0] = 0;
    type[0].expList[1] = 1 ;type[1].expList[1] = 1 ;type[2].expList[1] = 1 ;type[3].expList[1] = 1 ;type[4].expList[1] = 1 ;type[5].expList[1] = 1;
    type[0].expList[2] = 8 ;type[1].expList[2] = 15 ;type[2].expList[2] = 4 ;type[3].expList[2] = 9 ;type[4].expList[2] = 6 ;type[5].expList[2] = 10;
    type[0].expList[3] = 27 ;type[1].expList[3] = 52 ;type[2].expList[3] = 13 ;type[3].expList[3] = 57 ;type[4].expList[3] = 21 ;type[5].expList[3] = 33;
    type[0].expList[4] = 64 ;type[1].expList[4] = 122 ;type[2].expList[4] = 32 ;type[3].expList[4] = 96 ;type[4].expList[4] = 51 ;type[5].expList[4] = 80;
    type[0].expList[5] = 125 ;type[1].expList[5] = 237 ;type[2].expList[5] = 65 ;type[3].expList[5] = 135 ;type[4].expList[5] = 100 ;type[5].expList[5] = 156;
    type[0].expList[6] = 216 ;type[1].expList[6] = 406 ;type[2].expList[6] = 113 ;type[3].expList[6] = 179 ;type[4].expList[6] = 172 ;type[5].expList[6] = 270;
    type[0].expList[7] = 343 ;type[1].expList[7] = 637 ;type[2].expList[7] = 182 ;type[3].expList[7] = 236 ;type[4].expList[7] = 274 ;type[5].expList[7] = 428;
    type[0].expList[8] = 512 ;type[1].expList[8] = 942 ;type[2].expList[8] = 276 ;type[3].expList[8] = 314 ;type[4].expList[8] = 409 ;type[5].expList[8] = 640;
    type[0].expList[9] = 729 ;type[1].expList[9] = 1326 ;type[2].expList[9] = 398 ;type[3].expList[9] = 419 ;type[4].expList[9] = 583 ;type[5].expList[9] = 911;
    type[0].expList[10] = 1000 ;type[1].expList[10] = 1800 ;type[2].expList[10] = 553 ;type[3].expList[10] = 560 ;type[4].expList[10] = 800 ;type[5].expList[10] = 1250;
    type[0].expList[11] = 1331 ;type[1].expList[11] = 2369 ;type[2].expList[11] = 745 ;type[3].expList[11] = 742 ;type[4].expList[11] = 1064 ;type[5].expList[11] = 1663;
    type[0].expList[12] = 1728 ;type[1].expList[12] = 3041 ;type[2].expList[12] = 979 ;type[3].expList[12] = 973 ;type[4].expList[12] = 1382 ;type[5].expList[12] = 2160;
    type[0].expList[13] = 2197 ;type[1].expList[13] = 3822 ;type[2].expList[13] = 1259 ;type[3].expList[13] = 1261 ;type[4].expList[13] = 1757 ;type[5].expList[13] = 2746;
    type[0].expList[14] = 2744 ;type[1].expList[14] = 4719 ;type[2].expList[14] = 1591 ;type[3].expList[14] = 1612 ;type[4].expList[14] = 2195 ;type[5].expList[14] = 3430;
    type[0].expList[15] = 3375 ;type[1].expList[15] = 5737 ;type[2].expList[15] = 1980 ;type[3].expList[15] = 2035 ;type[4].expList[15] = 2700 ;type[5].expList[15] = 4218;
    type[0].expList[16] = 4096 ;type[1].expList[16] = 6881 ;type[2].expList[16] = 2457 ;type[3].expList[16] = 2535 ;type[4].expList[16] = 3276 ;type[5].expList[16] = 5120;
    type[0].expList[17] = 4913 ;type[1].expList[17] = 8155 ;type[2].expList[17] = 3046 ;type[3].expList[17] = 3120 ;type[4].expList[17] = 3930 ;type[5].expList[17] = 6141;
    type[0].expList[18] = 5832 ;type[1].expList[18] = 9564 ;type[2].expList[18] = 3732 ;type[3].expList[18] = 3798 ;type[4].expList[18] = 4665 ;type[5].expList[18] = 7290;
    type[0].expList[19] = 6859 ;type[1].expList[19] = 11111 ;type[2].expList[19] = 4526 ;type[3].expList[19] = 4575 ;type[4].expList[19] = 5487 ;type[5].expList[19] = 8573;
    type[0].expList[20] = 8000 ;type[1].expList[20] = 12800 ;type[2].expList[20] = 5440 ;type[3].expList[20] = 5460 ;type[4].expList[20] = 6400 ;type[5].expList[20] = 10000;
    type[0].expList[21] = 9261 ;type[1].expList[21] = 14632 ;type[2].expList[21] = 6482 ;type[3].expList[21] = 6458 ;type[4].expList[21] = 7408 ;type[5].expList[21] = 11576;
    type[0].expList[22] = 10648 ;type[1].expList[22] = 16610 ;type[2].expList[22] = 7666 ;type[3].expList[22] = 7577 ;type[4].expList[22] = 8518 ;type[5].expList[22] = 13310;
    type[0].expList[23] = 12167 ;type[1].expList[23] = 18737 ;type[2].expList[23] = 9003 ;type[3].expList[23] = 8825 ;type[4].expList[23] = 9733 ;type[5].expList[23] = 15208;
    type[0].expList[24] = 13824 ;type[1].expList[24] = 21012 ;type[2].expList[24] = 10506 ;type[3].expList[24] = 10208 ;type[4].expList[24] = 11059 ;type[5].expList[24] = 17280;
    type[0].expList[25] = 15625 ;type[1].expList[25] = 23437 ;type[2].expList[25] = 12187 ;type[3].expList[25] = 11735 ;type[4].expList[25] = 12500 ;type[5].expList[25] = 19531;
    type[0].expList[26] = 17576 ;type[1].expList[26] = 26012 ;type[2].expList[26] = 14060 ;type[3].expList[26] = 13411 ;type[4].expList[26] = 14060 ;type[5].expList[26] = 21970;
    type[0].expList[27] = 19683 ;type[1].expList[27] = 28737 ;type[2].expList[27] = 16140 ;type[3].expList[27] = 15244 ;type[4].expList[27] = 15746 ;type[5].expList[27] = 24603;
    type[0].expList[28] = 21952 ;type[1].expList[28] = 31610 ;type[2].expList[28] = 18439 ;type[3].expList[28] = 17242 ;type[4].expList[28] = 17561 ;type[5].expList[28] = 27440;
    type[0].expList[29] = 24389 ;type[1].expList[29] = 34632 ;type[2].expList[29] = 20974 ;type[3].expList[29] = 19411 ;type[4].expList[29] = 19511 ;type[5].expList[29] = 30486;
    type[0].expList[30] = 27000 ;type[1].expList[30] = 37800 ;type[2].expList[30] = 23760 ;type[3].expList[30] = 21760 ;type[4].expList[30] = 21600 ;type[5].expList[30] = 33750;
    type[0].expList[31] = 29791 ;type[1].expList[31] = 41111 ;type[2].expList[31] = 26811 ;type[3].expList[31] = 24294 ;type[4].expList[31] = 23832 ;type[5].expList[31] = 37238;
    type[0].expList[32] = 32768 ;type[1].expList[32] = 44564 ;type[2].expList[32] = 30146 ;type[3].expList[32] = 27021 ;type[4].expList[32] = 26214 ;type[5].expList[32] = 40960;
    type[0].expList[33] = 35937 ;type[1].expList[33] = 48155 ;type[2].expList[33] = 33780 ;type[3].expList[33] = 29949 ;type[4].expList[33] = 28749 ;type[5].expList[33] = 44921;
    type[0].expList[34] = 39304 ;type[1].expList[34] = 51881 ;type[2].expList[34] = 37731 ;type[3].expList[34] = 33084 ;type[4].expList[34] = 31443 ;type[5].expList[34] = 49130;
    type[0].expList[35] = 42875 ;type[1].expList[35] = 55737 ;type[2].expList[35] = 42017 ;type[3].expList[35] = 36435 ;type[4].expList[35] = 34300 ;type[5].expList[35] = 53593;
    type[0].expList[36] = 46656 ;type[1].expList[36] = 59719 ;type[2].expList[36] = 46656 ;type[3].expList[36] = 40007 ;type[4].expList[36] = 37324 ;type[5].expList[36] = 58320;
    type[0].expList[37] = 50653 ;type[1].expList[37] = 63822 ;type[2].expList[37] = 50653 ;type[3].expList[37] = 43808 ;type[4].expList[37] = 40522 ;type[5].expList[37] = 63316;
    type[0].expList[38] = 54872 ;type[1].expList[38] = 68041 ;type[2].expList[38] = 55969 ;type[3].expList[38] = 47846 ;type[4].expList[38] = 43897 ;type[5].expList[38] = 68590;
    type[0].expList[39] = 59319 ;type[1].expList[39] = 72369 ;type[2].expList[39] = 60505 ;type[3].expList[39] = 52127 ;type[4].expList[39] = 47455 ;type[5].expList[39] = 74148;
    type[0].expList[40] = 64000 ;type[1].expList[40] = 76800 ;type[2].expList[40] = 66560 ;type[3].expList[40] = 56660 ;type[4].expList[40] = 51200 ;type[5].expList[40] = 80000;
    type[0].expList[41] = 68921 ;type[1].expList[41] = 81326 ;type[2].expList[41] = 71677 ;type[3].expList[41] = 61450 ;type[4].expList[41] = 55136 ;type[5].expList[41] = 86151;
    type[0].expList[42] = 74088 ;type[1].expList[42] = 85942 ;type[2].expList[42] = 78533 ;type[3].expList[42] = 66505 ;type[4].expList[42] = 59270 ;type[5].expList[42] = 92610;
    type[0].expList[43] = 79507 ;type[1].expList[43] = 90637 ;type[2].expList[43] = 84277 ;type[3].expList[43] = 71833 ;type[4].expList[43] = 63605 ;type[5].expList[43] = 99383;
    type[0].expList[44] = 85184 ;type[1].expList[44] = 95406 ;type[2].expList[44] = 91998 ;type[3].expList[44] = 77440 ;type[4].expList[44] = 68147 ;type[5].expList[44] = 106480;
    type[0].expList[45] = 91125 ;type[1].expList[45] = 100237 ;type[2].expList[45] = 98415 ;type[3].expList[45] = 83335 ;type[4].expList[45] = 72900 ;type[5].expList[45] = 113906;
    type[0].expList[46] = 97336 ;type[1].expList[46] = 105122 ;type[2].expList[46] = 107069 ;type[3].expList[46] = 89523 ;type[4].expList[46] = 77868 ;type[5].expList[46] = 121670;
    type[0].expList[47] = 103823 ;type[1].expList[47] = 110052 ;type[2].expList[47] = 114205 ;type[3].expList[47] = 96012 ;type[4].expList[47] = 83058 ;type[5].expList[47] = 129778;
    type[0].expList[48] = 110592 ;type[1].expList[48] = 115015 ;type[2].expList[48] = 123863 ;type[3].expList[48] = 102810 ;type[4].expList[48] = 88473 ;type[5].expList[48] = 138240;
    type[0].expList[49] = 117649 ;type[1].expList[49] = 120001 ;type[2].expList[49] = 131766 ;type[3].expList[49] = 109923 ;type[4].expList[49] = 94119 ;type[5].expList[49] = 147061;
    type[0].expList[50] = 125000 ;type[1].expList[50] = 125000 ;type[2].expList[50] = 142500 ;type[3].expList[50] = 117360 ;type[4].expList[50] = 100000 ;type[5].expList[50] = 156250;
    type[0].expList[51] = 132651 ;type[1].expList[51] = 131324 ;type[2].expList[51] = 151222 ;type[3].expList[51] = 125126 ;type[4].expList[51] = 106120 ;type[5].expList[51] = 165813;
    type[0].expList[52] = 140608 ;type[1].expList[52] = 137795 ;type[2].expList[52] = 163105 ;type[3].expList[52] = 133229 ;type[4].expList[52] = 112486 ;type[5].expList[52] = 175760;
    type[0].expList[53] = 148877 ;type[1].expList[53] = 144410 ;type[2].expList[53] = 172697 ;type[3].expList[53] = 141677 ;type[4].expList[53] = 119101 ;type[5].expList[53] = 186096;
    type[0].expList[54] = 157464 ;type[1].expList[54] = 151165 ;type[2].expList[54] = 185807 ;type[3].expList[54] = 150476 ;type[4].expList[54] = 125971 ;type[5].expList[54] = 196830;
    type[0].expList[55] = 166375 ;type[1].expList[55] = 158056 ;type[2].expList[55] = 196322 ;type[3].expList[55] = 159635 ;type[4].expList[55] = 133100 ;type[5].expList[55] = 207968;
    type[0].expList[56] = 175616 ;type[1].expList[56] = 165079 ;type[2].expList[56] = 210739 ;type[3].expList[56] = 169159 ;type[4].expList[56] = 140492 ;type[5].expList[56] = 219520;
    type[0].expList[57] = 185193 ;type[1].expList[57] = 172229 ;type[2].expList[57] = 222231 ;type[3].expList[57] = 179056 ;type[4].expList[57] = 148154 ;type[5].expList[57] = 231491;
    type[0].expList[58] = 195112 ;type[1].expList[58] = 179503 ;type[2].expList[58] = 238036 ;type[3].expList[58] = 189334 ;type[4].expList[58] = 156089 ;type[5].expList[58] = 243890;
    type[0].expList[59] = 205379 ;type[1].expList[59] = 186894 ;type[2].expList[59] = 250562 ;type[3].expList[59] = 199999 ;type[4].expList[59] = 164303 ;type[5].expList[59] = 256723;
    type[0].expList[60] = 216000 ;type[1].expList[60] = 194400 ;type[2].expList[60] = 267840 ;type[3].expList[60] = 211060 ;type[4].expList[60] = 172800 ;type[5].expList[60] = 270000;
    type[0].expList[61] = 226981 ;type[1].expList[61] = 202013 ;type[2].expList[61] = 281456 ;type[3].expList[61] = 222522 ;type[4].expList[61] = 181584 ;type[5].expList[61] = 283726;
    type[0].expList[62] = 238328 ;type[1].expList[62] = 209728 ;type[2].expList[62] = 300293 ;type[3].expList[62] = 234393 ;type[4].expList[62] = 190662 ;type[5].expList[62] = 297910;
    type[0].expList[63] = 250047 ;type[1].expList[63] = 217540 ;type[2].expList[63] = 315059 ;type[3].expList[63] = 246681 ;type[4].expList[63] = 200037 ;type[5].expList[63] = 312558;
    type[0].expList[64] = 262144 ;type[1].expList[64] = 225443 ;type[2].expList[64] = 335544 ;type[3].expList[64] = 259392 ;type[4].expList[64] = 209715 ;type[5].expList[64] = 327680;
    type[0].expList[65] = 274625 ;type[1].expList[65] = 233431 ;type[2].expList[65] = 351520 ;type[3].expList[65] = 272535 ;type[4].expList[65] = 219700 ;type[5].expList[65] = 343281;
    type[0].expList[66] = 287496 ;type[1].expList[66] = 241496 ;type[2].expList[66] = 373744 ;type[3].expList[66] = 286115 ;type[4].expList[66] = 229996 ;type[5].expList[66] = 359370;
    type[0].expList[67] = 300763 ;type[1].expList[67] = 249633 ;type[2].expList[67] = 390991 ;type[3].expList[67] = 300140 ;type[4].expList[67] = 240610 ;type[5].expList[67] = 375953;
    type[0].expList[68] = 314432 ;type[1].expList[68] = 257834 ;type[2].expList[68] = 415050 ;type[3].expList[68] = 314618 ;type[4].expList[68] = 251545 ;type[5].expList[68] = 393040;
    type[0].expList[69] = 328509 ;type[1].expList[69] = 267406 ;type[2].expList[69] = 433631 ;type[3].expList[69] = 329555 ;type[4].expList[69] = 262807 ;type[5].expList[69] = 410636;
    type[0].expList[70] = 343000 ;type[1].expList[70] = 276458 ;type[2].expList[70] = 459620 ;type[3].expList[70] = 344960 ;type[4].expList[70] = 274400 ;type[5].expList[70] = 428750;
    type[0].expList[71] = 357911 ;type[1].expList[71] = 286328 ;type[2].expList[71] = 479600 ;type[3].expList[71] = 360838 ;type[4].expList[71] = 286328 ;type[5].expList[71] = 447388;
    type[0].expList[72] = 373248 ;type[1].expList[72] = 296358 ;type[2].expList[72] = 507617 ;type[3].expList[72] = 377197 ;type[4].expList[72] = 298598 ;type[5].expList[72] = 466560;
    type[0].expList[73] = 389017 ;type[1].expList[73] = 305767 ;type[2].expList[73] = 529063 ;type[3].expList[73] = 394045 ;type[4].expList[73] = 311213 ;type[5].expList[73] = 486271;
    type[0].expList[74] = 405224 ;type[1].expList[74] = 316074 ;type[2].expList[74] = 559209 ;type[3].expList[74] = 411388 ;type[4].expList[74] = 324179 ;type[5].expList[74] = 506530;
    type[0].expList[75] = 421875 ;type[1].expList[75] = 326531 ;type[2].expList[75] = 582187 ;type[3].expList[75] = 429235 ;type[4].expList[75] = 337500 ;type[5].expList[75] = 527343;
    type[0].expList[76] = 438976 ;type[1].expList[76] = 336255 ;type[2].expList[76] = 614566 ;type[3].expList[76] = 447591 ;type[4].expList[76] = 351180 ;type[5].expList[76] = 548720;
    type[0].expList[77] = 456533 ;type[1].expList[77] = 346965 ;type[2].expList[77] = 639146 ;type[3].expList[77] = 466464 ;type[4].expList[77] = 365226 ;type[5].expList[77] = 570666;
    type[0].expList[78] = 474552 ;type[1].expList[78] = 357812 ;type[2].expList[78] = 673863 ;type[3].expList[78] = 485862 ;type[4].expList[78] = 379641 ;type[5].expList[78] = 593190;
    type[0].expList[79] = 493039 ;type[1].expList[79] = 367807 ;type[2].expList[79] = 700115 ;type[3].expList[79] = 505791 ;type[4].expList[79] = 394431 ;type[5].expList[79] = 616298;
    type[0].expList[80] = 512000 ;type[1].expList[80] = 378880 ;type[2].expList[80] = 737280 ;type[3].expList[80] = 526260 ;type[4].expList[80] = 409600 ;type[5].expList[80] = 640000;
    type[0].expList[81] = 531441 ;type[1].expList[81] = 390077 ;type[2].expList[81] = 765275 ;type[3].expList[81] = 547274 ;type[4].expList[81] = 425152 ;type[5].expList[81] = 664301;
    type[0].expList[82] = 551368 ;type[1].expList[82] = 400293 ;type[2].expList[82] = 804997 ;type[3].expList[82] = 568841 ;type[4].expList[82] = 441094 ;type[5].expList[82] = 689210;
    type[0].expList[83] = 571787 ;type[1].expList[83] = 411686 ;type[2].expList[83] = 834809 ;type[3].expList[83] = 590969 ;type[4].expList[83] = 457429 ;type[5].expList[83] = 714733;
    type[0].expList[84] = 592704 ;type[1].expList[84] = 423190 ;type[2].expList[84] = 877201 ;type[3].expList[84] = 613664 ;type[4].expList[84] = 474163 ;type[5].expList[84] = 740880;
    type[0].expList[85] = 614125 ;type[1].expList[85] = 433572 ;type[2].expList[85] = 908905 ;type[3].expList[85] = 636935 ;type[4].expList[85] = 491300 ;type[5].expList[85] = 767656;
    type[0].expList[86] = 636056 ;type[1].expList[86] = 445239 ;type[2].expList[86] = 954084 ;type[3].expList[86] = 660787 ;type[4].expList[86] = 508844 ;type[5].expList[86] = 795070;
    type[0].expList[87] = 658503 ;type[1].expList[87] = 457001 ;type[2].expList[87] = 987754 ;type[3].expList[87] = 685228 ;type[4].expList[87] = 526802 ;type[5].expList[87] = 823128;
    type[0].expList[88] = 681472 ;type[1].expList[88] = 467489 ;type[2].expList[88] = 1035837 ;type[3].expList[88] = 710266 ;type[4].expList[88] = 545177 ;type[5].expList[88] = 851840;
    type[0].expList[89] = 704969 ;type[1].expList[89] = 479378 ;type[2].expList[89] = 1071552 ;type[3].expList[89] = 735907 ;type[4].expList[89] = 563975 ;type[5].expList[89] = 881211;
    type[0].expList[90] = 729000 ;type[1].expList[90] = 491346 ;type[2].expList[90] = 1122660 ;type[3].expList[90] = 762160 ;type[4].expList[90] = 583200 ;type[5].expList[90] = 911250;
    type[0].expList[91] = 753571 ;type[1].expList[91] = 501878 ;type[2].expList[91] = 1160499 ;type[3].expList[91] = 789030 ;type[4].expList[91] = 602856 ;type[5].expList[91] = 941963;
    type[0].expList[92] = 778688 ;type[1].expList[92] = 513934 ;type[2].expList[92] = 1214753 ;type[3].expList[92] = 816525 ;type[4].expList[92] = 622950 ;type[5].expList[92] = 973360;
    type[0].expList[93] = 804357 ;type[1].expList[93] = 526049 ;type[2].expList[93] = 1254796 ;type[3].expList[93] = 844653 ;type[4].expList[93] = 643485 ;type[5].expList[93] = 1005446;
    type[0].expList[94] = 830584 ;type[1].expList[94] = 536557 ;type[2].expList[94] = 1312322 ;type[3].expList[94] = 873420 ;type[4].expList[94] = 664467 ;type[5].expList[94] = 1038230;
    type[0].expList[95] = 857375 ;type[1].expList[95] = 548720 ;type[2].expList[95] = 1354652 ;type[3].expList[95] = 902835 ;type[4].expList[95] = 685900 ;type[5].expList[95] = 1071718;
    type[0].expList[96] = 884736 ;type[1].expList[96] = 560922 ;type[2].expList[96] = 1415577 ;type[3].expList[96] = 932903 ;type[4].expList[96] = 707788 ;type[5].expList[96] = 1105920;
    type[0].expList[97] = 912673 ;type[1].expList[97] = 571333 ;type[2].expList[97] = 1460276 ;type[3].expList[97] = 963632 ;type[4].expList[97] = 730138 ;type[5].expList[97] = 1140841;
    type[0].expList[98] = 941192 ;type[1].expList[98] = 583539 ;type[2].expList[98] = 1524731 ;type[3].expList[98] = 995030 ;type[4].expList[98] = 752953 ;type[5].expList[98] = 1176490;
    type[0].expList[99] = 970299 ;type[1].expList[99] = 591882 ;type[2].expList[99] = 1571884 ;type[3].expList[99] = 1027103 ;type[4].expList[99] = 776239 ;type[5].expList[99] = 1212873;
    type[0].expList[100] = 1000000 ;type[1].expList[100] = 600000 ;type[2].expList[100] = 1640000 ;type[3].expList[100] = 1059860 ;type[4].expList[100] = 800000 ;type[5].expList[100] = 1250000;
}
void Widget::loadType()
{
    pokeType[0].pix.load("://images/普.png");
    pokeType[1].pix.load("://images/格.png");
    pokeType[2].pix.load("://images/飞.png");
    pokeType[3].pix.load("://images/毒.png");
    pokeType[4].pix.load("://images/地.png");
    pokeType[5].pix.load("://images/岩.png");
    pokeType[6].pix.load("://images/虫.png");
    pokeType[7].pix.load("://images/鬼.png");
    pokeType[8].pix.load("://images/钢.png");
    pokeType[10].pix.load("://images/炎.png");
    pokeType[11].pix.load("://images/水.png");
    pokeType[12].pix.load("://images/草.png");
    pokeType[13].pix.load("://images/电.png");
    pokeType[14].pix.load("://images/超.png");
    pokeType[15].pix.load("://images/冰.png");
    pokeType[16].pix.load("://images/龙.png");
    pokeType[17].pix.load("://images/恶.png");
    pokeType[23].pix.load("://images/仙.png");
}

void Widget::loadAbility()
{
    ability[1] = tr("恶臭");
    ability[2] = tr("降雨");
    ability[3] = tr("加速");
    ability[4] = tr("战斗盔甲");
    ability[5] = tr("结实");
    ability[6] = tr("湿气");
    ability[7] = tr("柔软");
    ability[8] = tr("沙隐");
    ability[9] = tr("静电");
    ability[10] = tr("蓄电");
    ability[11] = tr("储水");
    ability[12] = tr("迟钝");
    ability[13] = tr("无关天气");
    ability[14] = tr("复眼");
    ability[15] = tr("不眠");
    ability[16] = tr("变色");
    ability[17] = tr("免疫");
    ability[18] = tr("引火");
    ability[19] = tr("鳞粉");
    ability[20] = tr("我行我素");
    ability[21] = tr("吸盘");
    ability[22] = tr("威吓");
    ability[23] = tr("踩影");
    ability[24] = tr("粗糙皮肤");
    ability[25] = tr("神奇守护");
    ability[26] = tr("飘浮");
    ability[27] = tr("孢子");
    ability[28] = tr("同步");
    ability[29] = tr("恒净之躯");
    ability[30] = tr("自然回复");
    ability[31] = tr("避雷针");
    ability[32] = tr("天恩");
    ability[33] = tr("悠游自如");
    ability[34] = tr("叶绿素");
    ability[35] = tr("发光");
    ability[36] = tr("复制");
    ability[37] = tr("大力士");
    ability[38] = tr("毒刺");
    ability[39] = tr("精神力");
    ability[40] = tr("熔岩铠甲");
    ability[41] = tr("水幕");
    ability[42] = tr("磁力");
    ability[43] = tr("隔音");
    ability[44] = tr("雨盘");
    ability[45] = tr("扬沙");
    ability[46] = tr("压迫感");
    ability[47] = tr("厚脂肪");
    ability[48] = tr("早起");
    ability[49] = tr("火焰之躯");
    ability[50] = tr("逃跑");
    ability[51] = tr("锐利目光");
    ability[52] = tr("怪力钳");
    ability[53] = tr("捡拾");
    ability[54] = tr("懒惰");
    ability[55] = tr("活力");
    ability[56] = tr("迷人之躯");
    ability[57] = tr("正电");
    ability[58] = tr("负电");
    ability[59] = tr("阴晴不定");
    ability[60] = tr("黏着");
    ability[61] = tr("蜕皮");
    ability[62] = tr("毅力");
    ability[63] = tr("神奇鳞片");
    ability[64] = tr("污泥浆");
    ability[65] = tr("茂盛");
    ability[66] = tr("猛火");
    ability[67] = tr("激流");
    ability[68] = tr("虫之预感");
    ability[69] = tr("坚硬脑袋");
    ability[70] = tr("日照");
    ability[71] = tr("沙穴");
    ability[72] = tr("干劲");
    ability[73] = tr("白色烟雾");
    ability[74] = tr("瑜伽之力");
    ability[75] = tr("硬壳盔甲");
    ability[76] = tr("气闸");
    ability[77] = tr("蹒跚");
    ability[78] = tr("电气引擎");
    ability[79] = tr("斗争心");
    ability[80] = tr("不屈之心");
    ability[81] = tr("雪隐");
    ability[82] = tr("贪吃鬼");
    ability[83] = tr("愤怒穴位");
    ability[84] = tr("轻装");
    ability[85] = tr("耐热");
    ability[86] = tr("单纯");
    ability[87] = tr("干燥皮肤");
    ability[88] = tr("下载");
    ability[89] = tr("铁拳");
    ability[90] = tr("毒疗");
    ability[91] = tr("适应力");
    ability[92] = tr("连续攻击");
    ability[93] = tr("湿润之躯");
    ability[94] = tr("太阳之力");
    ability[95] = tr("飞毛腿");
    ability[96] = tr("一般皮肤");
    ability[97] = tr("狙击手");
    ability[98] = tr("魔法防守");
    ability[99] = tr("无防守");
    ability[100] = tr("慢出");
    ability[101] = tr("技术高手");
    ability[102] = tr("叶子防守");
    ability[103] = tr("笨拙");
    ability[104] = tr("破格");
    ability[105] = tr("超幸运");
    ability[106] = tr("引爆");
    ability[107] = tr("危险预知");
    ability[108] = tr("预知梦");
    ability[109] = tr("纯朴");
    ability[110] = tr("有色眼镜");
    ability[111] = tr("过滤");
    ability[112] = tr("慢启动");
    ability[113] = tr("胆量");
    ability[114] = tr("引水");
    ability[115] = tr("冰冻之躯");
    ability[116] = tr("坚硬岩石");
    ability[117] = tr("降雪");
    ability[118] = tr("采蜜");
    ability[119] = tr("察觉");
    ability[120] = tr("舍身");
    ability[121] = tr("多属性");
    ability[122] = tr("花之礼");
    ability[123] = tr("梦魇");
    ability[124] = tr("顺手牵羊");
    ability[125] = tr("强行");
    ability[126] = tr("唱反调");
    ability[127] = tr("紧张感");
    ability[128] = tr("不服输");
    ability[129] = tr("软弱");
    ability[130] = tr("诅咒之躯");
    ability[131] = tr("治愈之心");
    ability[132] = tr("友情防守");
    ability[133] = tr("碎裂铠甲");
    ability[134] = tr("重金属");
    ability[135] = tr("轻金属");
    ability[136] = tr("多重鳞片");
    ability[137] = tr("中毒激升");
    ability[138] = tr("受热激升");
    ability[139] = tr("收获");
    ability[140] = tr("心灵感应");
    ability[141] = tr("心情不定");
    ability[142] = tr("防尘");
    ability[143] = tr("毒手");
    ability[144] = tr("再生力");
    ability[145] = tr("健壮胸肌");
    ability[146] = tr("拨沙");
    ability[147] = tr("奇迹皮肤");
    ability[148] = tr("分析");
    ability[149] = tr("幻觉");
    ability[150] = tr("变身者");
    ability[151] = tr("穿透");
    ability[152] = tr("木乃伊");
    ability[153] = tr("自信过度");
    ability[154] = tr("正义之心");
    ability[155] = tr("胆怯");
    ability[156] = tr("魔法镜");
    ability[157] = tr("食草");
    ability[158] = tr("恶作剧之心");
    ability[159] = tr("沙之力");
    ability[160] = tr("铁刺");
    ability[161] = tr("达摩模式");
    ability[162] = tr("胜利之星");
    ability[163] = tr("涡轮火焰");
    ability[164] = tr("兆级电压");
    ability[165] = tr("芳香幕");
    ability[166] = tr("花幕");
    ability[167] = tr("颊囊");
    ability[168] = tr("变幻自如");
    ability[169] = tr("毛皮大衣");
    ability[170] = tr("魔术师");
    ability[171] = tr("防弹");
    ability[172] = tr("好胜");
    ability[173] = tr("强壮之颚");
    ability[174] = tr("冰冻皮肤");
    ability[175] = tr("甜幕");
    ability[176] = tr("战斗切换");
    ability[177] = tr("疾风之翼");
    ability[178] = tr("超级发射器");
    ability[179] = tr("草之毛皮");
    ability[180] = tr("共生");
    ability[181] = tr("硬爪");
    ability[182] = tr("妖精皮肤");
    ability[183] = tr("黏滑");
    ability[184] = tr("飞行皮肤");
    ability[185] = tr("亲子爱");
    ability[186] = tr("暗黑气场");
    ability[187] = tr("妖精气场");
    ability[188] = tr("气场破坏");
    ability[189] = tr("始源之海");
    ability[190] = tr("终结之地");
    ability[191] = tr("德尔塔气流");
    ability[192] = tr("持久力");
    ability[193] = tr("跃跃欲逃");
    ability[194] = tr("危险回避");
    ability[195] = tr("遇水凝固");
    ability[196] = tr("不仁不义");
    ability[197] = tr("界限盾壳");
    ability[198] = tr("蹲守");
    ability[199] = tr("水泡");
    ability[200] = tr("钢能力者");
    ability[201] = tr("怒火冲天");
    ability[202] = tr("拨雪");
    ability[203] = tr("远隔");
    ability[204] = tr("湿润之声");
    ability[205] = tr("先行治疗");
    ability[206] = tr("电气皮肤");
    ability[207] = tr("冲浪之尾");
    ability[208] = tr("鱼群");
    ability[209] = tr("画皮");
    ability[210] = tr("牵绊变身");
    ability[211] = tr("群聚变形");
    ability[212] = tr("腐蚀");
    ability[213] = tr("绝对睡眠");
    ability[214] = tr("女王的威严");
    ability[215] = tr("飞出的内在物");
    ability[216] = tr("舞者");
    ability[217] = tr("蓄电池");
    ability[218] = tr("毛茸茸");
    ability[219] = tr("鲜艳之躯");
    ability[220] = tr("魂心");
    ability[221] = tr("卷发");
    ability[222] = tr("接球手");
    ability[223] = tr("化学之力");
    ability[224] = tr("异兽提升");
    ability[225] = tr("AR系统");
    ability[226] = tr("电气制造者");
    ability[227] = tr("精神制造者");
    ability[228] = tr("薄雾制造者");
    ability[229] = tr("青草制造者");
    ability[230] = tr("金属防护");
    ability[231] = tr("幻影防守");
    ability[232] = tr("棱镜装甲");
    ability[233] = tr("脑核之力");
}

void Widget::readFile()
{
//    QFile file1(QString("BreedNameList_4.txt"));
//    QFile file2(QString("ItemNameList_4.txt"));
    QFile file3(QString("AreaNameList.txt"));
    if(/*false == file1.open(QIODevice::ReadOnly) || false == file2.open(QIODevice::ReadOnly) || */false == file3.open(QIODevice::ReadOnly))//只读打开
    {
        QMessageBox::critical(this,"文件读取失败",tr("无法正确读取文件，\n"
                                               "请确保文件没有被删除！"),QMessageBox::Ok);
        exit(1);
    }
//    for(int i=0;!file1.atEnd();i++)
//    {
//        QString str = QString(file1.readLine());
//        breed[i].name = str.mid(6);
//        breed[i].name.chop(2);
//    }
//    for(int i=0;!file2.atEnd();i++)
//    {
//        QString str = QString(file2.readLine());
//        item[i].name = str.mid(6);
//        item[i].name.chop(2);
//    }
    for(int i=0;!file3.atEnd();i++)
    {
        QString str = QString(file3.readLine());
        area[i].name = str.mid(5);
        area[i].name.chop(2);
    }
//    file1.close();
//    file2.close();
    file3.close();
}

void Widget::addItem()
{
    VBA.Attach();
    u32 base;
    ReadProcessMemory(VBA.m_hProcess,(void*)0x005A8F58,&base,4,NULL);
    u8 data[44];
    u32 offset = base + 0xF2B79B - 11;
    //精灵名字
    for(int i=0;i<BREEDNUM;i++)
    {
//        QString str = QString::number(i,16);while(str.length()<3)str.insert(0,"0");str = str.toUpper();
//        ui->comboBreed->addItem(str + ": " + breed[i].name);
        //获取基址的内容，加上偏移量得到实际偏移
        ReadProcessMemory(VBA.m_hProcess,(void*)offset,data,11,NULL);
        offset += 11;
        char num[10];
        sprintf(num,"%03X:",i);
        QString name = decode(data,11);
        if(name == "") name = "-";
        breed[i].name = name;
        ui->comboBreed->addItem(num + breed[i].name);
    }
    //道具名字
    offset = base + 0xFC2CA8 - 44;
    for(int i=0;i<ITEMNUM;i++)
    {
//        QString str = QString::number(i,16);while(str.length()<3)str.insert(0,"0");str = str.toUpper();
//        ui->comboItem->addItem(str + ": " + item[i].name);
//        ui->comboPlayerItem->addItem(str + ": " + item[i].name);
        ReadProcessMemory(VBA.m_hProcess,(void*)offset,data,44,NULL);
        offset += 44;
        char num[10];
        sprintf(num,"%03X:",i);
        QString name = decode(data,11);
        if(name == "") name = "-";
        item[i].name = name;
        ui->comboItem->addItem(num + item[i].name);

    }
    for(int i=0;i<AREANUM;i++)
    {
        QString str = QString::number(i,16);while(str.length()<2)str.insert(0,"0");str = str.toUpper();
        ui->comboPlace->addItem(str + ": " + area[i].name);
    }
    ui->comboPers->addItems(Personality);
    QStringList PointUp;
    PointUp << ("0%") << ("20%") << ("40%") << ("60%");
    ui->comboPP1->addItems(PointUp);ui->comboPP2->addItems(PointUp);ui->comboPP3->addItems(PointUp);ui->comboPP4->addItems(PointUp);
    ui->comboInitialLevel->addItem(tr("蛋种"));
    for(int i=1;i<256;i++)
        ui->comboInitialLevel->addItem(tr("Lv.%1").arg(i));
}

void Widget::readPoke(int index)
{
    currentPoke.breed = VBA.ReadValue_2(poke[index].breedOffset);
    ui->comboBreed->setCurrentIndex(currentPoke.breed);                             //种族
    ui->comboItem->setCurrentIndex(VBA.ReadValue_2(poke[index].itemOffset));        //持有物
    currentPoke.pers = VBA.ReadValue_4(poke[index].PersOffset);
    ui->comboPers->setCurrentIndex(currentPoke.pers % 25);                          //性格
    QString str = QString::number(currentPoke.pers,16);while(str.length()<8) str.insert(0,"0"); str = str.toUpper();
    ui->linePersValue->setText(str);                                                //性格值
    str = QString::number(VBA.ReadValue_4(poke[index].ID),16);while(str.length()<8) str.insert(0,"0"); str = str.toUpper();
    ui->lineHideID->setText(str);                                                   //ID
    ui->lineInti->setText(QString::number(VBA.ReadValue_1(poke[index].intiOffset)));//亲密度
    showExpLevel();                                                                 //等级
    ui->lineExp->setText(QString::number(VBA.ReadValue_4(poke[index].ExpOffset)));  //经验值
    showEnd();                                                                      //努力值
    showId();                                                                       //个体
    showHideID();                                                                   //ID
    showBeauty();                                                                   //魅力
    showAbValue();                                                                  //能力值
    showTrim();                                                                     //强迫症
    showMark();                                                                     //标记
    showSex();                                                                      //性别
    showColor();                                                                    //闪光
    showPP();                                                                       //PP
    showEgg();                                                                      //蛋
    showEggSteps();                                                                 //孵蛋圈数
    showHide();                                                                     //隐藏特性
    showAbility();                                                                  //显示特性
    showPointUp();                                                                  //PP增幅
    showRibbon();                                                                   //缎带
    showPokeName();                                                                 //名字
    showOTName();                                                                   //主人名字
    showStatus();                                                                   //状态
    showMegaLock();                                                                 //显示是否锁定Mega

    showFixed();                                                                    //固定属性
    showBreedValue();                                                               //种族值
    refreshSkill();
}
void Widget::showExpLevel()
{
    ui->comboLevel->clear();
    currentPoke.level = VBA.ReadValue_1(poke[currentPoke.index].levelOffset);
    currentPoke.expType = VBA.ReadValue_1(0x00F186E0 + 28*currentPoke.breed + 19,VBA.ROMAddress);
    for(int i=0;i<=100;i++)
    {
        QString str = QString("Lv.%1 :%2").arg(i).arg(type[currentPoke.expType].expList[i]);
        ui->comboLevel->addItem(str);
    }
    ui->comboLevel->setCurrentIndex(currentPoke.level);
}
void Widget::showEnd()//努力值
{
    currentPoke.end_hp = VBA.ReadValue_1(poke[currentPoke.index].end_hp);
    currentPoke.end_atk = VBA.ReadValue_1(poke[currentPoke.index].end_atk);
    currentPoke.end_def = VBA.ReadValue_1(poke[currentPoke.index].end_def);
    currentPoke.end_sp = VBA.ReadValue_1(poke[currentPoke.index].end_sp);
    currentPoke.end_spatk = VBA.ReadValue_1(poke[currentPoke.index].end_spatk);
    currentPoke.end_spdef = VBA.ReadValue_1(poke[currentPoke.index].end_spdef);
    ui->end_hp->setText(QString::number(currentPoke.end_hp));
    ui->end_atk->setText(QString::number(currentPoke.end_atk));
    ui->end_def->setText(QString::number(currentPoke.end_def));
    ui->end_sp->setText(QString::number(currentPoke.end_sp));
    ui->end_spatk->setText(QString::number(currentPoke.end_spatk));
    ui->end_spdef->setText(QString::number(currentPoke.end_spdef));
}
void Widget::showId()//个体
{
    currentPoke.ability  = VBA.ReadValue_4(poke[currentPoke.index].abiliOffset);
    QString str = QString::number(currentPoke.ability,2);
    while(str.length() < 32)//补齐32位
        str.insert(0,'0');
    QString spdef = str.mid(2,5);currentPoke.IVs_spdef = spdef.toInt(nullptr,2);
    QString spatk = str.mid(7,5);currentPoke.IVs_spatk = spatk.toInt(nullptr,2);
    QString sp = str.mid(12,5);currentPoke.IVs_sp = sp.toInt(nullptr,2);
    QString def = str.mid(17,5);currentPoke.IVs_def = def.toInt(nullptr,2);
    QString atk = str.mid(22,5);currentPoke.IVs_atk = atk.toInt(nullptr,2);
    QString hp = str.mid(27,5);currentPoke.IVs_hp = hp.toInt(nullptr,2);
    ui->ab_hp->setText(QString::number(currentPoke.IVs_hp));
    ui->ab_atk->setText(QString::number(currentPoke.IVs_atk));
    ui->ab_def->setText(QString::number(currentPoke.IVs_def));
    ui->ab_sp->setText(QString::number(currentPoke.IVs_sp));
    ui->ab_spatk->setText(QString::number(currentPoke.IVs_spatk));
    ui->ab_spdef->setText(QString::number(currentPoke.IVs_spdef));
}
void Widget::showHideID()//ID
{

}
void Widget::showBeauty()//魅力值
{
    currentPoke.b1 = VBA.ReadValue_1(poke[currentPoke.index].beauty1);
    currentPoke.b2 = VBA.ReadValue_1(poke[currentPoke.index].beauty2);
    currentPoke.b3 = VBA.ReadValue_1(poke[currentPoke.index].beauty3);
    currentPoke.b4 = VBA.ReadValue_1(poke[currentPoke.index].beauty4);
    currentPoke.b5 = VBA.ReadValue_1(poke[currentPoke.index].beauty5);
    currentPoke.b6 = VBA.ReadValue_1(poke[currentPoke.index].beauty6);
    ui->b1->setText(QString::number(currentPoke.b1));
    ui->b2->setText(QString::number(currentPoke.b2));
    ui->b3->setText(QString::number(currentPoke.b3));
    ui->b4->setText(QString::number(currentPoke.b4));
    ui->b5->setText(QString::number(currentPoke.b5));
    ui->b6->setText(QString::number(currentPoke.b6));
}
void Widget::showAbValue()//实际值
{
    currentPoke.totalHp = VBA.ReadValue_2(poke[currentPoke.index].totalHP);
    currentPoke.Atk = VBA.ReadValue_2(poke[currentPoke.index].Atk);
    currentPoke.Def = VBA.ReadValue_2(poke[currentPoke.index].Def);
    currentPoke.Speed = VBA.ReadValue_2(poke[currentPoke.index].Speed);
    currentPoke.SpAtk = VBA.ReadValue_2(poke[currentPoke.index].SpAtk);
    currentPoke.SpDef = VBA.ReadValue_2(poke[currentPoke.index].SpDef);
    ui->hp->setText(QString::number(currentPoke.totalHp));
    ui->atk->setText(QString::number(currentPoke.Atk));
    ui->def->setText(QString::number(currentPoke.Def));
    ui->sp->setText(QString::number(currentPoke.Speed));
    ui->spatk->setText(QString::number(currentPoke.SpAtk));
    ui->spdef->setText(QString::number(currentPoke.SpDef));
}
void Widget::showTrim()//强迫症
{
    u32 trim = VBA.ReadValue_4(poke[currentPoke.index].virusOffset);
    QString str = QString::number(trim,2);
    while(str.length()<32)str.insert(0,"0");
    //宠物球
    int ball = str.mid(1,5).toInt(nullptr,2);
    ui->comboBall->setCurrentIndex(ball);
    //初始等级
    int level = str.mid(8,8).toInt(nullptr,2);
    ui->comboInitialLevel->setCurrentIndex(level);
    //捕获地
    if(str[0] == '0')ui->placeProbably->setChecked(true);
    else if(str[0] == '1')ui->placeProbably->setChecked(false);
    int place = str.mid(16,8).toInt(nullptr,2);
    ui->comboPlace->setCurrentIndex(place);
    //宝可病毒
    QString black = str.mid(24,4);
    QString virus = str.mid(28,4);
    u8 b = 0,v = 0;
    if(black[0] == '1') b += 8;if(black[1] == '1') b += 4;if(black[2] == '1') b += 2;if(black[3] == '1') b += 1;
    if(virus[0] == '1') v += 8;if(virus[1] == '1') v += 4;if(virus[2] == '1') v += 2;if(virus[3] == '1') v += 1;
    if(b == 0 && v == 0)
        ui->comboVirus->setCurrentIndex(0);//无病毒
    else if(v != 0)
        ui->comboVirus->setCurrentIndex(1);//显示病毒
    else if(b != 0)
        ui->comboVirus->setCurrentIndex(2);//显示黑点
}
void Widget::showMark()
{
    currentPoke.mark = VBA.ReadValue_1(poke[currentPoke.index].markOffset);
    int n = currentPoke.mark;
    char b[] = "0000";
    for(int i=3;i>=0;i--)
    {
        int k = n%2;
        n/=2;
        if(k)
            b[i] = '1';
        else
            b[i] = '0';
    }
    if(b[0] == '0')ui->mark1->setChecked(false);else ui->mark1->setChecked(true);
    if(b[1] == '0')ui->mark2->setChecked(false);else ui->mark2->setChecked(true);
    if(b[2] == '0')ui->mark3->setChecked(false);else ui->mark3->setChecked(true);
    if(b[3] == '0')ui->mark4->setChecked(false);else ui->mark4->setChecked(true);
}
void Widget::showSex()
{
    currentPoke.sexRatio = VBA.ReadValue_1(0x00F186E0 + currentPoke.breed*28 +16,VBA.ROMAddress);
    ui->comboSex->clear();
    u8 sex = ui->linePersValue->text().mid(6,2).toInt(nullptr,16);
    if(currentPoke.sexRatio == 0xFF)        //无性别
    {
        ui->comboSex->addItem(tr("无性别"));
    }
    else if(currentPoke.sexRatio == 0x00)   //只有公
    {
        ui->comboSex->addItem(tr("♂"));
    }
    else if(currentPoke.sexRatio == 0xFE)   //只有母
    {
        ui->comboSex->addItem(tr("♀"));
    }
    else
    {
        ui->comboSex->addItem(tr("♂"));
        ui->comboSex->addItem(tr("♀"));
        currentPoke.sex = sex >= currentPoke.sexRatio ? 0 : 1;
        ui->comboSex->setCurrentIndex(currentPoke.sex);
    }
}
void Widget::showColor()
{
    u32 ID = VBA.ReadValue_4(poke[currentPoke.index].ID);
    u32 Per = VBA.ReadValue_4(poke[currentPoke.index].PersOffset);
    u16 n1 = (ID & 4294901760) >> 16;
    u16 n2 = (ID & 65535);
    u16 n = n1 ^ n2;
    u16 m1 = (Per & 4294901760) >> 16;
    u16 m2 = (Per & 65535);
    u16 m = m1 ^ m2;
    if((m ^ n) < 7)
        ui->changeColor->setChecked(true);
    else
        ui->changeColor->setChecked(false);
}
void Widget::showSkill_1()
{
    VBA.Attach();
    u32 base;
    ReadProcessMemory(VBA.m_hProcess,(void*)0x005A8F58,&base,4,NULL);
    u8 data[13];
    u32 offset = base + 0x1d1d84d - 13;
    if(!readedSkill)//没有读取过技能文件 就读取 //借用这和flag，但是不读取文件了
    {
//        QFile file3(QString("SkillNameList_4.txt"));
//        if(false == file3.open(QIODevice::ReadOnly))
//        {
//            QMessageBox::critical(this,"文件读取失败",tr("无法正确读取文件，\n"
//                                                   "请确保文件没有被删除！"),QMessageBox::Ok);
//            exit(1);
//        }
        //添加技能名字

//        for(int i=0;!file3.atEnd();i++)
//        {
//            QString str = QString(file3.readLine());
//            skill[i].name = str.mid(6);
//            skill[i].name.chop(2);
//        }

        for(int i=0;i<SKILLNUM;i++)
        {
//            QString str = QString::number(i,16);while(str.length()<3)str.insert(0,"0");str = str.toUpper();
//            ui->skill1->addItem(str + ": " + skill[i].name);
//            ui->skill2->addItem(str + ": " + skill[i].name);
            ReadProcessMemory(VBA.m_hProcess,(void*)offset,data,13,NULL);
            offset += 13;
            char num[10];
            sprintf(num,"%03X:",i);
            QString name = decode(data,13);
            if(name == "")name = "-";
            skill[i].name = name;
            ui->skill1->addItem(num + skill[i].name);
            ui->skill2->addItem(num + skill[i].name);

        }
//        file3.close();
        readedSkill = true;
    }
}
void Widget::showSkill_2()
{
    if(!addedSkill)
        for(int i=0;i<SKILLNUM;i++)
        {
             QString str = QString::number(i,16);while(str.length()<3)str.insert(0,"0");str = str.toUpper();
             ui->skill3->addItem(str + ":" + skill[i].name);
             ui->skill4->addItem(str + ":" + skill[i].name);
        }
    refreshSkill();
    addedSkill = true;
}
void Widget::showEgg()
{
    currentPoke.ability = VBA.ReadValue_4(poke[currentPoke.index].abiliOffset);
    QString str = QString::number(currentPoke.ability,2);
    while(str.length() < 32)
        str.insert(0,'0');//获取个体值
    if(str[1] == '0')
        ui->toEgg->setChecked(false);
    else if(str[1] == '1')
        ui->toEgg->setChecked(true);
}
void Widget::showEggSteps()
{
    //判断是不是蛋
    QString str = QString::number(currentPoke.ability,2);
    while(str.length() < 32)
        str.insert(0,'0');//获取个体值
    if(str[1] == '0')
    {
        ui->lineRestEggSteps->setText(tr("XXX"));
    }
    else if(str[1] == '1')
    {
        ui->lineRestEggSteps->setText(ui->lineInti->text());
    }
}
void Widget::showHide()
{
    currentPoke.hideID = VBA.ReadValue_2(poke[currentPoke.index].HideID);
    u16 p = currentPoke.hideID;
    QString str = QString::number(p,2);
    while(str.length()<16)
        str.insert(0,"0");
    if(str[15] == '1')
        ui->changeHideId->setChecked(true);
    else if(str[15] == '0')
        ui->changeHideId->setChecked(false);
}
void Widget::showAbility()
{
    u8 a1 = VBA.ReadValue_1(0x00F186E0+28*currentPoke.breed+22,VBA.ROMAddress);
    u8 a2 = VBA.ReadValue_1(0x00F186E0+28*currentPoke.breed+23,VBA.ROMAddress);
    u8 a3 = VBA.ReadValue_1(0x00F186E0+28*currentPoke.breed+26,VBA.ROMAddress);
    ui->comboAbility->clear();
    ui->comboAbility->addItem(ability[a1]);
    ui->comboAbility->addItem(ability[a2]);
    ui->comboAbility->addItem(tr("梦特: ") + ability[a3]);
    currentPoke.hideID = VBA.ReadValue_2(poke[currentPoke.index].HideID);
    u16 p = currentPoke.hideID;
    QString str = QString::number(p,2);
    while(str.length()<16)
        str.insert(0,"0");
    if(str[15] == '1')//是隐藏特性
        ui->comboAbility->setCurrentIndex(2);
    else if(str[15] == '0')
    {
        QString a = QString::number(VBA.ReadValue_4(poke[currentPoke.index].abiliOffset),2);
        while(a.length() < 32)a.insert(0,"0");
//        qDebug() << a;
        u8 ab = QString(a[0]).toInt();
        ui->comboAbility->setCurrentIndex(ab);
    }
}
void Widget::showPP()
{
    currentPoke.PP1 = VBA.ReadValue_1(poke[currentPoke.index].PP1);
    currentPoke.PP2 = VBA.ReadValue_1(poke[currentPoke.index].PP2);
    currentPoke.PP3 = VBA.ReadValue_1(poke[currentPoke.index].PP3);
    currentPoke.PP4 = VBA.ReadValue_1(poke[currentPoke.index].PP4);
    ui->PP1->setText(QString::number(currentPoke.PP1));
    ui->PP2->setText(QString::number(currentPoke.PP2));
    ui->PP3->setText(QString::number(currentPoke.PP3));
    ui->PP4->setText(QString::number(currentPoke.PP4));
}
void Widget::showPointUp()
{
    int n;
    n = currentPoke.PointUp = VBA.ReadValue_1(poke[currentPoke.index].PointUp);
    int a[4] = {0};
    for(int i=0;i<4;i++)
    {
        a[i] = n%4;
        n /= 4;
    }
    ui->comboPP1->setCurrentIndex(a[0]);ui->comboPP2->setCurrentIndex(a[1]);ui->comboPP3->setCurrentIndex(a[2]);ui->comboPP4->setCurrentIndex(a[3]);
}
void Widget::showRibbon()
{
    u32 r = VBA.ReadValue_4(poke[currentPoke.index].Ribbon);
    QString str = QString::number(r,2);
    while(str.length() < 32) str.insert(0,"0");
    QString unk = str.mid(5.7);
    QString other = str.mid(12,5);
    int s = 0;
    int b1 = str.mid(17,3).toInt(nullptr,2);
    int b2 = str.mid(20,3).toInt(nullptr,2);
    int b3 = str.mid(23,3).toInt(nullptr,2);
    int b4 = str.mid(26,3).toInt(nullptr,2);
    int b5 = str.mid(29,3).toInt(nullptr,2);
    s = b1 + b2 + b3 + b4 + b5;
    //contest
    if(b5==0)ui->ribbon_b1_0->setChecked(true);else if(b5==1)ui->ribbon_b1_1->setChecked(true);else if(b5==2)ui->ribbon_b1_2->setChecked(true);else if(b5==3)ui->ribbon_b1_3->setChecked(true);else if(b5==4)ui->ribbon_b1_4->setChecked(true);
    if(b4==0)ui->ribbon_b2_0->setChecked(true);else if(b4==1)ui->ribbon_b2_1->setChecked(true);else if(b4==2)ui->ribbon_b2_2->setChecked(true);else if(b4==3)ui->ribbon_b2_3->setChecked(true);else if(b4==4)ui->ribbon_b2_4->setChecked(true);
    if(b3==0)ui->ribbon_b3_0->setChecked(true);else if(b3==1)ui->ribbon_b3_1->setChecked(true);else if(b3==2)ui->ribbon_b3_2->setChecked(true);else if(b3==3)ui->ribbon_b3_3->setChecked(true);else if(b3==4)ui->ribbon_b3_4->setChecked(true);
    if(b2==0)ui->ribbon_b4_0->setChecked(true);else if(b2==1)ui->ribbon_b4_1->setChecked(true);else if(b2==2)ui->ribbon_b4_2->setChecked(true);else if(b2==3)ui->ribbon_b4_3->setChecked(true);else if(b2==4)ui->ribbon_b4_4->setChecked(true);
    if(b1==0)ui->ribbon_b5_0->setChecked(true);else if(b1==1)ui->ribbon_b5_1->setChecked(true);else if(b1==2)ui->ribbon_b5_2->setChecked(true);else if(b1==3)ui->ribbon_b5_3->setChecked(true);else if(b1==4)ui->ribbon_b5_4->setChecked(true);
    //unk
    if(unk[6] == '1') {ui->ribbon_unk_1->setChecked(true);s++;}else if(unk[6] == '0')ui->ribbon_unk_1->setChecked(false);
    if(unk[5] == '1') {ui->ribbon_unk_2->setChecked(true);s++;}else if(unk[5] == '0')ui->ribbon_unk_2->setChecked(false);
    if(unk[4] == '1') {ui->ribbon_unk_3->setChecked(true);s++;}else if(unk[4] == '0')ui->ribbon_unk_3->setChecked(false);
    if(unk[3] == '1') {ui->ribbon_unk_4->setChecked(true);s++;}else if(unk[3] == '0')ui->ribbon_unk_4->setChecked(false);
    if(unk[2] == '1') {ui->ribbon_unk_5->setChecked(true);s++;}else if(unk[2] == '0')ui->ribbon_unk_5->setChecked(false);
    if(unk[1] == '1') {ui->ribbon_unk_6->setChecked(true);s++;}else if(unk[1] == '0')ui->ribbon_unk_6->setChecked(false);
    if(unk[0] == '1') {ui->ribbon_unk_7->setChecked(true);s++;}else if(unk[0] == '0')ui->ribbon_unk_7->setChecked(false);
    //other
    if(other[4] == '1') {ui->ribbon_other_1->setChecked(true);s++;}else if(other[4] == '0') ui->ribbon_other_1->setChecked(false);
    if(other[3] == '1') {ui->ribbon_other_2->setChecked(true);s++;}else if(other[3] == '0') ui->ribbon_other_2->setChecked(false);
    if(other[2] == '1') {ui->ribbon_other_3->setChecked(true);s++;}else if(other[2] == '0') ui->ribbon_other_3->setChecked(false);
    if(other[1] == '1') {ui->ribbon_other_4->setChecked(true);s++;}else if(other[1] == '0') ui->ribbon_other_4->setChecked(false);
    if(other[0] == '1') {ui->ribbon_other_5->setChecked(true);s++;}else if(other[0] == '0') ui->ribbon_other_5->setChecked(false);
    ui->ribbonNum->display(s);
}
void Widget::showPokeName()
{
    u32 address;//临时存放地址的变量
    ReadProcessMemory(VBA.m_hProcess, (LPVOID)0x005A8F50, &address, 4, NULL);//第一次读取
    address += (poke[currentPoke.index].begin + 8);//计算真实地址
    ReadProcessMemory(VBA.m_hProcess, (LPVOID)address, currentPoke.name, 10, NULL);
    QString str;
    for(int i=0;i<10;i++)
    {
        QString s = QString::number(currentPoke.name[i],16);
        while(s.length()<2)s.insert(0,"0");
        if(!(i==9)) s += " ";
        str += s;
    }
    str = str.toUpper();
    ui->nameInput->setText("");
    ui->name->setText(str);
}
void Widget::showOTName()
{
    u32 address;//临时存放地址的变量
    ReadProcessMemory(VBA.m_hProcess, (LPVOID)0x005A8F50, &address, 4, NULL);//第一次读取
    address += (poke[currentPoke.index].begin + 20);//计算真实地址
    ReadProcessMemory(VBA.m_hProcess, (LPVOID)address, currentPoke.OTName, 7, NULL);
    QString str;
    for(int i=0;i<7;i++)
    {
        QString s = QString::number(currentPoke.OTName[i],16);
        while(s.length()<2)s.insert(0,"0");
        if(!(i==6)) s += " ";
        str += s;
    }
    str = str.toUpper();
    ui->lineOTNameInput->setText("");
    ui->lineOTName->setText(str);
}
void Widget::showStatus()
{
    currentPoke.status = VBA.ReadValue_1(poke[currentPoke.index].Status);
    QString str = QString::number(currentPoke.status,2);
    while(str.length()<8)str.insert(0,"0");
    if(str[0]=='1')ui->status_toxic->setChecked(true);else if(str[0]=='0')ui->status_toxic->setChecked(false);
    if(str[1]=='1')ui->status_palsy->setChecked(true);else if(str[1]=='0')ui->status_palsy->setChecked(false);
    if(str[2]=='1')ui->status_frost->setChecked(true);else if(str[2]=='0')ui->status_frost->setChecked(false);
    if(str[3]=='1')ui->status_burn->setChecked(true);else if(str[3]=='0')ui->status_burn->setChecked(false);
    if(str[4]=='1')ui->status_poison->setChecked(true);else if(str[4]=='0')ui->status_poison->setChecked(false);
    str = str.mid(5,3);
    int bout = str.toInt(nullptr,2);
    str = QString::number(bout,10);
    if(bout != 0)ui->status_sleep->setChecked(true);else ui->status_sleep->setChecked(false);
    ui->lineSleepBout->setText(str);
}

void Widget::showFixed()
{
    //名字
    ui->fix_name->setText(breed[currentPoke.breed].name);
    //属性
    u8 t1 = VBA.ReadValue_1(0x00F186E0 + 28*currentPoke.breed + 6,VBA.ROMAddress);
    u8 t2 = VBA.ReadValue_1(0x00F186E0 + 28*currentPoke.breed + 7,VBA.ROMAddress);
    if(t1<=24 && t1!=9 && !(t1>=18 && t1<=22))//属性合法
    {
        ui->type1->setPixmap(pokeType[t1].pix);
        ui->type2->setPixmap(QPixmap(""));
    }
    if(t2!=t1 && t2<=24 && t2!=9 && !(t2>=18 && t2<=22))
        ui->type2->setPixmap(pokeType[t2].pix);
    //性别比例
    switch(currentPoke.sexRatio)
    {
        case 0xFF:ui->label_sexratio->setText(tr("无性别"));break;
        case 0x00:ui->label_sexratio->setText(tr("100%雄性"));break;
        case 0xFE:ui->label_sexratio->setText(tr("100%雌性"));break;
        case 0x7F:ui->label_sexratio->setText(tr("50%雄性 50%雌性"));break;
        case 0xBF:ui->label_sexratio->setText(tr("25%雄性 75%雌性"));break;
        case 0x3F:ui->label_sexratio->setText(tr("75%雄性 25%雌性"));break;
        case 0x1F:ui->label_sexratio->setText(tr("87.5%雄性 12.5%雌性"));break;
    }
    //满级经验值
    switch(currentPoke.expType)
    {
        case 0:ui->label_MaxExp->setText("1,000,000");break;
        case 1:ui->label_MaxExp->setText("600,000");break;
        case 2:ui->label_MaxExp->setText("1,640,000");break;
        case 3:ui->label_MaxExp->setText("1,059,860");break;
        case 4:ui->label_MaxExp->setText("800,000");break;
        case 5:ui->label_MaxExp->setText("1,250,000");break;
    }
    //孵化周期
    u8 c = VBA.ReadValue_1(0x00F186E0 + 28*currentPoke.breed + 17,VBA.ROMAddress);
    QString str = QString("%1").arg(c) + tr("圈") + QString("(%1").arg(c*256) + tr("步)");
    ui->label_EggSteps->setText(str);
}
void Widget::showBreedValue()
{
    for (int i = 0; i < 6; i++)
        currentPoke.v[i] = VBA.ReadValue_1(0x00F186E0 + 28*currentPoke.breed + i,VBA.ROMAddress);
    QPixmap pix(364,197);
    pix.fill(Qt::white);
    QPainter painter(&pix);
    QPen p_line(QColor(204, 204, 204)),p_hp(QColor(2, 115, 36)),p_atk(QColor(114, 109, 0)),p_def(QColor(104, 55, 0)),p_sp(QColor(95, 2, 104)),p_spatk(QColor(4, 107, 109)),p_spdef(QColor(1, 32, 111)),p_sum(QColor(212, 99, 161)),p_maxValue(QColor(0,0,0));
    QBrush b_hp(QColor(18, 251, 88)),b_atk(QColor(255, 246, 12)),b_def(QColor(255, 135, 1)),b_sp(QColor(230, 8, 251)),b_spatk(QColor(20, 241, 246)),b_spdef(QColor(13, 80, 252));
    int w_value=100,w_text=5,w_num=60,h_value=7,h_line=28,h_text=22,i;
    //line
    painter.setPen(p_line);
    for(h_line=28,i=0;i<6;h_line+=28,i++)
        painter.drawLine(0,h_line,364,h_line);
    painter.drawLine(53,0,53,197);
    //text
    QFont font(tr("微软雅黑"),14,QFont::ExtraLight,false);
    painter.setFont(font);
    painter.setPen(p_hp);painter.drawText(18,h_text,tr("HP"));painter.drawText(w_num,h_text,QString::number(currentPoke.v[0]));h_text += 28;
    painter.setPen(p_atk);painter.drawText(w_text,h_text,tr("攻击"));painter.drawText(w_num,h_text,QString::number(currentPoke.v[1]));h_text += 28;
    painter.setPen(p_def);painter.drawText(w_text,h_text,tr("防御"));painter.drawText(w_num,h_text,QString::number(currentPoke.v[2]));h_text += 28;
    painter.setPen(p_spatk);painter.drawText(w_text,h_text,tr("特攻"));painter.drawText(w_num,h_text,QString::number(currentPoke.v[4]));h_text += 28;
    painter.setPen(p_spdef);painter.drawText(w_text,h_text,tr("特防"));painter.drawText(w_num,h_text,QString::number(currentPoke.v[5]));h_text += 28;
    painter.setPen(p_sp);painter.drawText(w_text,h_text,tr("速度"));painter.drawText(w_num,h_text,QString::number(currentPoke.v[3]));h_text += 28;
    painter.setPen(p_sum);painter.drawText(w_text,h_text,tr("总和"));painter.drawText(w_num,h_text,QString::number(currentPoke.v[0]+currentPoke.v[1]+currentPoke.v[2]+currentPoke.v[3]+currentPoke.v[4]+currentPoke.v[5]));
    //value
    painter.setPen(p_hp);painter.setBrush(b_hp);painter.drawRect(w_value,h_value,currentPoke.v[0],14);h_value += 28;
    painter.setPen(p_atk);painter.setBrush(b_atk);painter.drawRect(w_value,h_value,currentPoke.v[1],14);h_value += 28;
    painter.setPen(p_def);painter.setBrush(b_def);painter.drawRect(w_value,h_value,currentPoke.v[2],14);h_value += 28;
    painter.setPen(p_spatk);painter.setBrush(b_spatk);painter.drawRect(w_value,h_value,currentPoke.v[4],14);h_value += 28;
    painter.setPen(p_spdef);painter.setBrush(b_spdef);painter.drawRect(w_value,h_value,currentPoke.v[5],14);h_value += 28;
    painter.setPen(p_sp);painter.setBrush(b_sp);painter.drawRect(w_value,h_value,currentPoke.v[3],14);
    //maxValue
    painter.setPen(p_maxValue);
    painter.setFont(font);
    h_text = 22;
    if(currentPoke.v[0]==1)painter.drawText(313,h_text,QString::number(1));else
    painter.drawText(313,h_text,QString::number((int)(currentPoke.v[0]*2 + 31 + 252/4)*100/100 + 10 + 100));h_text += 28;
    painter.drawText(313,h_text,QString::number((int)(((currentPoke.v[1]*2 + 31 + 252/4)*100/100 + 5)*1.1)));h_text += 28;
    painter.drawText(313,h_text,QString::number((int)(((currentPoke.v[2]*2 + 31 + 252/4)*100/100 + 5)*1.1)));h_text += 28;
    painter.drawText(313,h_text,QString::number((int)(((currentPoke.v[4]*2 + 31 + 252/4)*100/100 + 5)*1.1)));h_text += 28;
    painter.drawText(313,h_text,QString::number((int)(((currentPoke.v[5]*2 + 31 + 252/4)*100/100 + 5)*1.1)));h_text += 28;
    painter.drawText(313,h_text,QString::number((int)(((currentPoke.v[3]*2 + 31 + 252/4)*100/100 + 5)*1.1)));
    ui->breedValuePanel->setPixmap(pix);
}
void Widget::showMegaLock()
{
    if(poke[currentPoke.index].megaLocked)
    {
        ui->lockMega->setChecked(true);
        ui->lockMega->setText(tr("还原状态(一次战斗后还原)"));
    }
    else
    {
        ui->lockMega->setChecked(false);
        ui->lockMega->setText(tr("锁定Mega形态(存在Mega形态)"));
    }
}

void Widget::on_refresh_clicked()//刷新按钮
{
    if(!readedFile)
    {
        readFile();
        readedFile = true;
    }
    if(!addedFlag)
    {
        addItem();
        addedFlag = true;
    }
    if(VBA.Attach())
    {
        qDebug() << "VBA的pid " << VBA.m_pid;
        readPoke(currentPoke.index);
    }
    else//异常处理
    {
        QMessageBox::critical(this,"MyPokeHack",tr("无法访问VBA的内存，\n"
                                               "可能是VBA版本不正确，\n"
                                               "或者游戏根本就没打开！"),QMessageBox::Ok);
        return;
//        exit(1);
    }
    ui->poke1->setText(breed[VBA.ReadValue_2(poke[1].breedOffset)].name);
    ui->poke2->setText(breed[VBA.ReadValue_2(poke[2].breedOffset)].name);
    ui->poke3->setText(breed[VBA.ReadValue_2(poke[3].breedOffset)].name);
    ui->poke4->setText(breed[VBA.ReadValue_2(poke[4].breedOffset)].name);
    ui->poke5->setText(breed[VBA.ReadValue_2(poke[5].breedOffset)].name);
    ui->poke6->setText(breed[VBA.ReadValue_2(poke[6].breedOffset)].name);
}

void Widget::on_refreshPlayer_clicked()
{
    if(VBA.Attach())
    {
        qDebug() << "VBA的pid " << VBA.m_pid;
        showPlayerName();
        showPlayerID();
        showMoney();
        showCoin();
        showDust();
        showRestSteps();
        showBattlePoint();
        showBattlePointOnCard();
        ui->lineNameInput->setText("");
    }
    else
    {
        QMessageBox::critical(this,"MyPokeHack",tr("无法访问VBA的内存，\n"
                                               "可能是VBA版本不正确，\n"
                                               "或者游戏根本就没打开！"),QMessageBox::Ok);
        exit(1);
    }
}

void Widget::on_writePlayer_clicked()
{
    writePlayerName();
    writePlayerID();
    writeMoney();
    writeCoin();
    writeDust();
    writeRestSteps();
    writeBattlePoint();
    writeBattlePointOnCard();
    on_refreshPlayer_clicked();
}

void Widget::on_write_clicked()//写入按钮
{
    currentPoke.breed = ui->comboBreed->currentIndex();
    VBA.WriteValue_2(poke[currentPoke.index].breedOffset,&currentPoke.breed);   //种族
    currentPoke.Item = ui->comboItem->currentIndex();
    VBA.WriteValue_2(poke[currentPoke.index].itemOffset,&currentPoke.Item);     //持有物
    currentPoke.inti = ui->lineInti->text().toInt();
    VBA.WriteValue_1(poke[currentPoke.index].intiOffset,&currentPoke.inti);     //亲密度
    currentPoke.exp = ui->lineExp->text().toInt();
    VBA.WriteValue_4(poke[currentPoke.index].ExpOffset,&currentPoke.exp);       //经验值
    currentPoke.level = ui->comboLevel->currentIndex();
    VBA.WriteValue_1(poke[currentPoke.index].levelOffset,&currentPoke.level);   //等级
    writeEnd();                                                                 //努力值
    writeId();                                                                  //个体
    writeHideID();                                                              //ID
    writeBeauty();                                                              //魅力
    writeAbValue();                                                             //能力值
    writeTrim();                                                                //强迫症
    writeMark();                                                                //标记
    writeAbility();                                                             //特性
    writePP();                                                                  //PP
    writePointUp();                                                             //PP增幅
    writeRibbon();                                                              //缎带
    writeStatus();                                                              //状态

    writePersValue();                                                           //性格值（性格，性别，闪光）
    on_refresh_clicked();
    QMessageBox::about(this,tr("提示"),tr("写入成功！\n请前往游戏查看！"));
}
void Widget::writeEnd()//努力值
{
    currentPoke.end_hp = ui->end_hp->text().toInt();
    currentPoke.end_atk = ui->end_atk->text().toInt();
    currentPoke.end_def = ui->end_def->text().toInt();
    currentPoke.end_sp = ui->end_sp->text().toInt();
    currentPoke.end_spatk = ui->end_spatk->text().toInt();
    currentPoke.end_spdef = ui->end_spdef->text().toInt();
    //判断努力值之和是否大于510
    int sum = currentPoke.end_hp + currentPoke.end_atk + currentPoke.end_def + currentPoke.end_sp + currentPoke.end_spatk + currentPoke.end_spdef;
    if(sum > 510)
    {
        //发出警告，但不影响修改
        QMessageBox::warning(this,tr("警告"),tr("当前努力值总和已超过510\n战斗后系统会清零努力值\n请调至努力值不超过510\n注：此次修改没有受到影响"));
    }
    VBA.WriteValue_1(poke[currentPoke.index].end_hp,&currentPoke.end_hp);
    VBA.WriteValue_1(poke[currentPoke.index].end_atk,&currentPoke.end_atk);
    VBA.WriteValue_1(poke[currentPoke.index].end_def,&currentPoke.end_def);
    VBA.WriteValue_1(poke[currentPoke.index].end_sp,&currentPoke.end_sp);
    VBA.WriteValue_1(poke[currentPoke.index].end_spatk,&currentPoke.end_spatk);
    VBA.WriteValue_1(poke[currentPoke.index].end_spdef,&currentPoke.end_spdef);
}
void Widget::writeId()//个体值
{
    //保留前两位特性
    currentPoke.pers = VBA.ReadValue_4(poke[currentPoke.index].abiliOffset);
    QString str = QString::number(currentPoke.pers,2);
    while(str.length() < 32)
        str.insert(0,'0');
    str = str.mid(0,2);
    QString hp = QString::number(ui->ab_hp->text().toInt(),2);while(hp.length() <5) hp.insert(0,'0');
    QString atk = QString::number(ui->ab_atk->text().toInt(),2);while(atk.length() <5) atk.insert(0,'0');
    QString def = QString::number(ui->ab_def->text().toInt(),2);while(def.length() <5) def.insert(0,'0');
    QString sp = QString::number(ui->ab_sp->text().toInt(),2);while(sp.length() <5) sp.insert(0,'0');
    QString spatk = QString::number(ui->ab_spatk->text().toInt(),2);while(spatk.length() <5) spatk.insert(0,'0');
    QString spdef = QString::number(ui->ab_spdef->text().toInt(),2);while(spdef.length() <5) spdef.insert(0,'0');
    str = str + spdef + spatk + sp + def + atk + hp;
    currentPoke.pers = 0;
    for(int i=0;i<32;i++)
    {
        if(str[i] == '1')
            currentPoke.pers += pow(2,32-i-1);
    }
    VBA.WriteValue_4(poke[currentPoke.index].abiliOffset,&currentPoke.pers);
}
void Widget::writeHideID()
{
    u32 id = ui->lineHideID->text().toUInt(nullptr,16);
    VBA.WriteValue_4(poke[currentPoke.index].ID,&id);
}
void Widget::writeBeauty()//魅力值
{
    currentPoke.b1 = ui->b1->text().toInt();
    currentPoke.b2 = ui->b2->text().toInt();
    currentPoke.b3 = ui->b3->text().toInt();
    currentPoke.b4 = ui->b4->text().toInt();
    currentPoke.b5 = ui->b5->text().toInt();
    currentPoke.b6 = ui->b6->text().toInt();
    VBA.WriteValue_1(poke[currentPoke.index].beauty1,&currentPoke.b1);
    VBA.WriteValue_1(poke[currentPoke.index].beauty2,&currentPoke.b2);
    VBA.WriteValue_1(poke[currentPoke.index].beauty3,&currentPoke.b3);
    VBA.WriteValue_1(poke[currentPoke.index].beauty4,&currentPoke.b4);
    VBA.WriteValue_1(poke[currentPoke.index].beauty5,&currentPoke.b5);
    VBA.WriteValue_1(poke[currentPoke.index].beauty6,&currentPoke.b6);
}
void Widget::writeAbValue()//能力值
{
    currentPoke.totalHp = ui->hp->text().toInt();
    currentPoke.Atk = ui->atk->text().toInt();
    currentPoke.Def = ui->def->text().toInt();
    currentPoke.Speed = ui->sp->text().toInt();
    currentPoke.SpAtk = ui->spatk->text().toInt();
    currentPoke.SpDef = ui->spdef->text().toInt();
    VBA.WriteValue_2(poke[currentPoke.index].totalHP,&currentPoke.totalHp);
    VBA.WriteValue_2(poke[currentPoke.index].Atk,&currentPoke.Atk);
    VBA.WriteValue_2(poke[currentPoke.index].Def,&currentPoke.Def);
    VBA.WriteValue_2(poke[currentPoke.index].Speed,&currentPoke.Speed);
    VBA.WriteValue_2(poke[currentPoke.index].SpAtk,&currentPoke.SpAtk);
    VBA.WriteValue_2(poke[currentPoke.index].SpDef,&currentPoke.SpDef);
}
void Widget::writeTrim()//强迫症
{
    u32 v =0;
    u32 trim = VBA.ReadValue_4(poke[currentPoke.index].virusOffset);
    QString str = QString::number(trim,2);
    while(str.length()<32)str.insert(0,"0");
    QString version = str.mid(6,2);
    QString probably;
    //宠物球
    int ball = ui->comboBall->currentIndex();
    QString s_ball = QString::number(ball,2);while(s_ball.length()<5)s_ball.insert(0,"0");
    //初始等级
    int level = ui->comboInitialLevel->currentIndex();
    QString s_level = QString::number(level,2);while(s_level.length()<8)s_level.insert(0,"0");
    //捕获地
    if(ui->placeProbably->isChecked()) probably = "0";
    else probably = "1";
    int place = ui->comboPlace->currentIndex();
    QString s_place = QString::number(place,2);while(s_place.length()<8)s_place.insert(0,"0");
    if(place==0xFF || place==0xFE)
        probably = "0";
    //宝可病毒
    QString s_virus;
    if(ui->comboVirus->currentIndex() == 0)//无
        s_virus = tr("00000000");
    else if(ui->comboVirus->currentIndex() == 1)//病毒
        s_virus = tr("00001111");
    else if(ui->comboVirus->currentIndex() == 2)//黑点
        s_virus = tr("11110000");
    QString value = probably + s_ball + version + s_level + s_place + s_virus;
    v = value.toUInt(nullptr,2);
    VBA.WriteValue_4(poke[currentPoke.index].virusOffset,&v);
}
void Widget::writeMark()
{
    char b[] = "0000"; u8 m = 0;
    if(ui->mark1->isChecked())  {b[0] = '1';m+=8;}
    if(ui->mark2->isChecked())  {b[1] = '1';m+=4;}
    if(ui->mark3->isChecked())  {b[2] = '1';m+=2;}
    if(ui->mark4->isChecked())  {b[3] = '1';m+=1;}
    VBA.WriteValue_1(poke[currentPoke.index].markOffset,&m);
    if(b[0]) m = 0;//无意义代码，不提示警告
}

void Widget::writeAbility()
{
    int s = ui->comboAbility->currentIndex();
    currentPoke.hideID = VBA.ReadValue_2(poke[currentPoke.index].HideID);
    QString str = QString::number(currentPoke.hideID,2);
    while(str.length()<16)str.insert(0,"0");
    if(s == 2)//隐藏特性
    {
        QString str1 = str.mid(0,15) + QString("1");
        currentPoke.hideID = str1.toUInt(nullptr,2);
        VBA.WriteValue_2(poke[currentPoke.index].HideID,&currentPoke.hideID);
    }
    else//普通特性
    {
        //取消隐藏特性
        QString str1 = str.mid(0,15) + QString("0");
        currentPoke.hideID = str1.toUInt(nullptr,2);
        VBA.WriteValue_2(poke[currentPoke.index].HideID,&currentPoke.hideID);
        //设置选中的特性
        QString a = QString::number(VBA.ReadValue_4(poke[currentPoke.index].abiliOffset),2);//读取特性值
        while(a.length()<32)a.insert(0,"0");//补齐32位
        a = a.mid(1);//去掉前一位特性位
        a = QString::number(s,2) + a;
        u32 v = a.toUInt(nullptr,2);
        VBA.WriteValue_4(poke[currentPoke.index].abiliOffset,&v);
    }
}
void Widget::writePP()
{
    currentPoke.PP1 = ui->PP1->text().toInt();
    currentPoke.PP2 = ui->PP2->text().toInt();
    currentPoke.PP3 = ui->PP3->text().toInt();
    currentPoke.PP4 = ui->PP4->text().toInt();
    VBA.WriteValue_1(poke[currentPoke.index].PP1,&currentPoke.PP1);
    VBA.WriteValue_1(poke[currentPoke.index].PP2,&currentPoke.PP2);
    VBA.WriteValue_1(poke[currentPoke.index].PP3,&currentPoke.PP3);
    VBA.WriteValue_1(poke[currentPoke.index].PP4,&currentPoke.PP4);
}
void Widget::writePointUp()
{
    u8 s1 , s2, s3, s4;
    s1 = ui->comboPP1->currentIndex();
    s2 = ui->comboPP2->currentIndex();
    s3 = ui->comboPP3->currentIndex();
    s4 = ui->comboPP4->currentIndex();
    u8 s = s1 + s2*pow(4,1) + s3*pow(4,2) + s4*pow(4,3);
    VBA.WriteValue_1(poke[currentPoke.index].PointUp,&s);
}
void Widget::writeRibbon()
{
    u32 r = VBA.ReadValue_4(poke[currentPoke.index].Ribbon);
    QString str = QString::number(r,2);
    while(str.length()<32)str.insert(0,"0");
    QString front = str.mid(0,5);//保留前5位
    int b1,b2,b3,b4,b5;
    bool u[7];bool o[5];
    if(ui->ribbon_b1_0->isChecked())b1=0;else if(ui->ribbon_b1_1->isChecked())b1=1;else if(ui->ribbon_b1_2->isChecked())b1=2;else if(ui->ribbon_b1_3->isChecked())b1=3;else if(ui->ribbon_b1_4->isChecked())b1=4;
    if(ui->ribbon_b2_0->isChecked())b2=0;else if(ui->ribbon_b2_1->isChecked())b2=1;else if(ui->ribbon_b2_2->isChecked())b2=2;else if(ui->ribbon_b2_3->isChecked())b2=3;else if(ui->ribbon_b2_4->isChecked())b2=4;
    if(ui->ribbon_b3_0->isChecked())b3=0;else if(ui->ribbon_b3_1->isChecked())b3=1;else if(ui->ribbon_b3_2->isChecked())b3=2;else if(ui->ribbon_b3_3->isChecked())b3=3;else if(ui->ribbon_b3_4->isChecked())b3=4;
    if(ui->ribbon_b4_0->isChecked())b4=0;else if(ui->ribbon_b4_1->isChecked())b4=1;else if(ui->ribbon_b4_2->isChecked())b4=2;else if(ui->ribbon_b4_3->isChecked())b4=3;else if(ui->ribbon_b4_4->isChecked())b4=4;
    if(ui->ribbon_b5_0->isChecked())b5=0;else if(ui->ribbon_b5_1->isChecked())b5=1;else if(ui->ribbon_b5_2->isChecked())b5=2;else if(ui->ribbon_b5_3->isChecked())b5=3;else if(ui->ribbon_b5_4->isChecked())b5=4;
    u[6]=ui->ribbon_unk_1->isChecked();u[5]=ui->ribbon_unk_2->isChecked();u[4]=ui->ribbon_unk_3->isChecked();u[3]=ui->ribbon_unk_4->isChecked();u[2]=ui->ribbon_unk_5->isChecked();u[1]=ui->ribbon_unk_6->isChecked();u[0]=ui->ribbon_unk_7->isChecked();
    o[4]=ui->ribbon_other_1->isChecked();o[3]=ui->ribbon_other_2->isChecked();o[2]=ui->ribbon_other_3->isChecked();o[1]=ui->ribbon_other_4->isChecked();o[0]=ui->ribbon_other_5->isChecked();
    QString other(5,'0');
    QString unk(7,'0');
    for(int i=0;i<7;i++)
        if(u[i])
            unk[i] = '1';
    for(int i=0;i<5;i++)
        if(o[i])
            other[i] = '1';
    QString sb1(""),sb2(""),sb3(""),sb4(""),sb5("");
    sb1 = QString::number(b1,2);while(sb1.length()<3)sb1.insert(0,"0");sb2 = QString::number(b2,2);while(sb2.length()<3)sb2.insert(0,"0");sb3 = QString::number(b3,2);while(sb3.length()<3)sb3.insert(0,"0");
    sb4 = QString::number(b4,2);while(sb4.length()<3)sb4.insert(0,"0");sb5 = QString::number(b5,2);while(sb5.length()<3)sb5.insert(0,"0");
    str = front + unk + other + sb5 + sb4 + sb3 + sb2 + sb1;
    r = str.toUInt(nullptr,2);
    VBA.WriteValue_4(poke[currentPoke.index].Ribbon,&r);
}
void Widget::writeStatus()
{
    QString str("00000");
    QString s("000");
    if(ui->status_toxic->isChecked())str[0]='1';
    if(ui->status_palsy->isChecked())str[1]='1';
    if(ui->status_frost->isChecked())str[2]='1';
    if(ui->status_burn->isChecked())str[3]='1';
    if(ui->status_poison->isChecked())str[4]='1';
    if(ui->status_sleep->isChecked())
    {
        int bout = ui->lineSleepBout->text().toInt(nullptr,10);
        s = QString::number(bout,2);
        while(s.length()<3)s.insert(0,"0");
    }
    str += s;
    while(str.length()>8)str.chop(1);
    currentPoke.status = str.toUShort(nullptr,2);
    VBA.WriteValue_1(poke[currentPoke.index].Status,&currentPoke.status);
}

void Widget::writePersValue()
{
    QString str = ui->linePersValue->text();
    u32 p = str.toUInt(nullptr,16);
    VBA.WriteValue_4(poke[currentPoke.index].PersOffset,&p);
}

void Widget::refreshSkill()
{
    currentPoke.skill_1 = VBA.ReadValue_2(poke[currentPoke.index].skill_1);
    currentPoke.skill_2 = VBA.ReadValue_2(poke[currentPoke.index].skill_2);
    currentPoke.skill_3 = VBA.ReadValue_2(poke[currentPoke.index].skill_3);
    currentPoke.skill_4 = VBA.ReadValue_2(poke[currentPoke.index].skill_4);
    ui->skill1->setCurrentIndex(currentPoke.skill_1);
    ui->skill2->setCurrentIndex(currentPoke.skill_2);
    ui->skill3->setCurrentIndex(currentPoke.skill_3);
    ui->skill4->setCurrentIndex(currentPoke.skill_4);
}

void Widget::showPlayerName()
{
    u8 name[7];
    u32 dwBase;
    u32 dwPointer1,dwPointer2;
    u32 Offset = 0x00005D90;
    ReadProcessMemory(VBA.m_hProcess, (LPVOID)0x005A8F54, &dwPointer1, 4, NULL);
    ReadProcessMemory(VBA.m_hProcess, (LPVOID)0x005A8F50, &dwPointer2, 4, NULL);
    ReadProcessMemory(VBA.m_hProcess, (LPVOID)(dwPointer1 + Offset), &dwBase, 4, NULL);
    Offset = ((dwBase) & 0x00FFFFFF);
    ReadProcessMemory(VBA.m_hProcess, (LPVOID)(dwPointer2 + Offset), name, 7, NULL);
    /************************************/
    QString str;
    for(int i=0;i<7;i++)
    {
        QString s = QString::number(name[i],16);
        while(s.length()<2)s.insert(0,"0");
        if(!(i == 6))
            s += " ";
        str += s;
    }
    str = str.toUpper();
    ui->linePlayerName->setText(str);
}
void Widget::showPlayerID()
{
    u32 PlayerID;
    PlayerID = VBA.PtrRead_4(0x0000000A);
    QString str = QString::number(PlayerID,16);
    while(str.length()<8)str.insert(0,"0");
    str = str.toUpper();
    ui->linePlayerID->setText("0x" + str);
}
void Widget::showMoney()
{
    u32 money;
    money = VBA.PtrReadXor_4(0x0000143C);
    QString str = QString::number(money);
    ui->lineMoney->setText(str);
}
void Widget::showCoin()
{
    u16 coin;
    coin = VBA.PtrReadXor_2(0x00001440);
    QString str = QString::number(coin);
    ui->lineCoin->setText(str);
}
void Widget::showDust()
{
    u16 dust;
    dust = VBA.PtrRead_2(0x000023D8);
    QString str = QString::number(dust);
    ui->lineDust->setText(str);
}
void Widget::showRestSteps()
{
    u16 steps;
    steps = VBA.ReadValue_2(0x3A04E);
    QString str = QString::number(steps);
    ui->lineRestSteps->setText(str);
}
void Widget::showBattlePoint()
{
    u16 point;
    point = VBA.PtrRead_2(0x00000EB8);
    QString str = QString::number(point);
    ui->lineBattlePoint->setText(str);
}
void Widget::showBattlePointOnCard()
{
    u16 point;
    point = VBA.PtrRead_2(0x00000EBA);
    QString str = QString::number(point);
    ui->lineBattlePointOnCard->setText(str);
}

void Widget::writePlayerName()
{
    QString str = ui->linePlayerName->text();
    u8 name[7];
    name[0] = str.mid(0,2).toUShort(nullptr,16);
    name[1] = str.mid(3,2).toUShort(nullptr,16);
    name[2] = str.mid(6,2).toUShort(nullptr,16);
    name[3] = str.mid(9,2).toUShort(nullptr,16);
    name[4] = str.mid(12,2).toUShort(nullptr,16);
    name[5] = str.mid(15,2).toUShort(nullptr,16);
    name[6] = str.mid(18,2).toUShort(nullptr,16);
    u32 dwBase;
    u32 dwPointer1,dwPointer2;
    u32 Offset = 0x00005D90;
    ReadProcessMemory(VBA.m_hProcess, (LPVOID)0x005A8F54, &dwPointer1, 4, NULL);
    ReadProcessMemory(VBA.m_hProcess, (LPVOID)0x005A8F50, &dwPointer2, 4, NULL);
    ReadProcessMemory(VBA.m_hProcess, (LPVOID)(dwPointer1 + Offset), &dwBase, 4, NULL);
    Offset = ((dwBase) & 0x00FFFFFF);
    WriteProcessMemory(VBA.m_hProcess, (LPVOID)(dwPointer2 + Offset), name, 7, NULL);
}
void Widget::writePlayerID()
{
    u32 id;
    QString str = ui->linePlayerID->text();
    if(str.mid(0,2) == "0x")
        str = str.mid(2);
    id = str.toUInt(nullptr,16);
    VBA.PtrWrite_4(0x0000000A,&id);
}
void Widget::writeMoney()
{
    u32 money = ui->lineMoney->text().toUInt(nullptr,10);
    VBA.PtrWriteXor_4(0x0000143C,&money);
}
void Widget::writeCoin()
{
    u16 coin = ui->lineCoin->text().toUInt(nullptr,10);
    VBA.PtrWriteXor_2(0x00001440,&coin);
}
void Widget::writeDust()
{
    u16 dust = ui->lineDust->text().toUShort(nullptr,10);
    VBA.PtrWrite_2(0x000023D8,&dust);
}
void Widget::writeRestSteps()
{
    u16 steps = ui->lineRestSteps->text().toUShort(nullptr,10);
    VBA.WriteValue_2(0x3A04E,&steps);
}
void Widget::writeBattlePoint()
{
    u16 point = ui->lineBattlePoint->text().toUShort(nullptr,10);
    VBA.PtrWrite_2(0x00000EB8,&point);
}
void Widget::writeBattlePointOnCard()
{
    u16 point = ui->lineBattlePointOnCard->text().toUShort(nullptr,10);
    VBA.PtrWrite_2(0x00000EBA,&point);
}

void Widget::on_poke1_clicked()
{
    ui->screen->setText(tr("当前为第一位"));
    currentPoke.index = 1;
    ui->poke2->setChecked(false);ui->poke3->setChecked(false);ui->poke4->setChecked(false);ui->poke5->setChecked(false);ui->poke6->setChecked(false);
    ui->poke1->setChecked(true);
    readPoke(1);
}

void Widget::on_poke2_clicked()
{
    ui->screen->setText(tr("当前为第二位"));
    currentPoke.index = 2;
    ui->poke1->setChecked(false);ui->poke3->setChecked(false);ui->poke4->setChecked(false);ui->poke5->setChecked(false);ui->poke6->setChecked(false);
    ui->poke2->setChecked(true);
    readPoke(2);
}

void Widget::on_poke3_clicked()
{
    ui->screen->setText(tr("当前为第三位"));
    currentPoke.index = 3;
    ui->poke2->setChecked(false);ui->poke1->setChecked(false);ui->poke4->setChecked(false);ui->poke5->setChecked(false);ui->poke6->setChecked(false);
    ui->poke3->setChecked(true);
    readPoke(3);
}

void Widget::on_poke4_clicked()
{
    ui->screen->setText(tr("当前为第四位"));
    currentPoke.index = 4;
    ui->poke2->setChecked(false);ui->poke3->setChecked(false);ui->poke1->setChecked(false);ui->poke5->setChecked(false);ui->poke6->setChecked(false);
    ui->poke4->setChecked(true);
    readPoke(4);
}

void Widget::on_poke5_clicked()
{
    ui->screen->setText(tr("当前为第五位"));
    currentPoke.index = 5;
    ui->poke2->setChecked(false);ui->poke3->setChecked(false);ui->poke4->setChecked(false);ui->poke1->setChecked(false);ui->poke6->setChecked(false);
    ui->poke5->setChecked(true);
    readPoke(5);
}

void Widget::on_poke6_clicked()
{
    ui->screen->setText(tr("当前为第六位"));
    currentPoke.index = 6;
    ui->poke2->setChecked(false);ui->poke3->setChecked(false);ui->poke4->setChecked(false);ui->poke5->setChecked(false);ui->poke1->setChecked(false);
    ui->poke6->setChecked(true);
    readPoke(6);
}

void Widget::on_searchBreed_clicked()
{
    QString sub = ui->comboBreed->currentText();
    for(int i=0;i<BREEDNUM;i++)
    {
        if(breed[i].name.contains(sub))
        {
            ui->comboBreed->setCurrentIndex(i);
            break;
        }
    }
}
void Widget::on_searchItem_clicked()
{
    QString sub = ui->comboItem->currentText();
    for(int i=0;i<ITEMNUM;i++)
    {
        if(item[i].name.contains(sub))
        {
            ui->comboItem->setCurrentIndex(i);
            break;
        }
    }
}
void Widget::on_searchPlayerItem_clicked()
{
    QString sub = ui->comboPlayerItem->currentText();
    for(int i=0;i<ITEMNUM;i++)
    {
        if(item[i].name.contains(sub))
        {
            ui->comboPlayerItem->setCurrentIndex(i);
            break;
        }
    }
}
void Widget::on_searchSkill1_clicked()
{
    QString sub = ui->skill1->currentText();
    for(int i=0;i<SKILLNUM;i++)
    {
        if(skill[i].name.contains(sub))
        {
            ui->skill1->setCurrentIndex(i);
            break;
        }
    }
}
void Widget::on_searchSkill2_clicked()
{
    QString sub = ui->skill2->currentText();
    for(int i=0;i<SKILLNUM;i++)
    {
        if(skill[i].name.contains(sub))
        {
            ui->skill2->setCurrentIndex(i);
            break;
        }
    }
}
void Widget::on_searchSkill3_clicked()
{
    QString sub = ui->skill3->currentText();
    for(int i=0;i<SKILLNUM;i++)
    {
        if(skill[i].name.contains(sub))
        {
            ui->skill3->setCurrentIndex(i);
            break;
        }
    }
}
void Widget::on_searchSkill4_clicked()
{
    QString sub = ui->skill4->currentText();
    for(int i=0;i<SKILLNUM;i++)
    {
        if(skill[i].name.contains(sub))
        {
            ui->skill4->setCurrentIndex(i);
            break;
        }
    }
}

void Widget::on_changeColor_clicked()
{
    if(currentPoke.breed == 0)//无宝可梦
        return;
    if(ui->changeColor->isChecked())//改闪光
    {
        u32 id = VBA.ReadValue_4(poke[currentPoke.index].ID);//获取ID值
        u32 per = VBA.ReadValue_4(poke[currentPoke.index].PersOffset);//获取性格值
        u32 p = per % 25;//获取原性格
        u16 n1 = (id & 4294901760) >> 16;
        u16 n2 = (id & 65535);
        u16 n = (n1 ^ n2);
        u16 m = n;
        int sum = 0;
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        while(1)
        {
            sum++;
            u16 m1 = qrand() % 65535;
            u16 m2 = ~((m1&m)|(~m1&~m));
            per = m1 << 16;
            per += m2;
            if(per%25 == p)
            {
                //需要判断性别
                if(currentPoke.sexRatio!=0xFF && currentPoke.sexRatio!=0xFE && currentPoke.sexRatio!=0x00)
                {
                    qDebug() << "需要判断";
                    u8 s = per;
                    if(currentPoke.sex == 0)//公
                    {
                        if(s >= currentPoke.sexRatio)
                        {
                            VBA.WriteValue_4(poke[currentPoke.index].PersOffset,&per);
                            break;
                        }
                    }
                    else if(currentPoke.sex == 1)//母
                    {
                        if(s < currentPoke.sexRatio)
                        {
                            VBA.WriteValue_4(poke[currentPoke.index].PersOffset,&per);
                            break;
                        }
                    }
                    else
                        continue;
                }
                else//不需要判断性别
                {
                    VBA.WriteValue_4(poke[currentPoke.index].PersOffset,&per);
                    break;
                }
            }
        }
        qDebug() << sum << "次";
        on_refresh_clicked();
    }
    else//破闪光
    {
        if(ui->comboPers->currentIndex() == 24)
        {
            on_comboPers_currentIndexChanged(ui->comboPers->currentIndex()-1);
            on_comboPers_currentIndexChanged(ui->comboPers->currentIndex());
        }
        else
        {
            on_comboPers_currentIndexChanged(ui->comboPers->currentIndex()+1);
            on_comboPers_currentIndexChanged(ui->comboPers->currentIndex());
        }
        on_write_clicked();
    }
}

void Widget::on_comboLevel_currentIndexChanged(int index)
{
    ui->lineExp->setText(QString::number(type[currentPoke.expType].expList[index]));
}

void Widget::on_tab_pokeinfo_currentChanged(int index)
{
    if(index == 2)
    {
        showSkill_2();
    }
}

void Widget::on_changeSkill_clicked()
{
    currentPoke.skill_1 = ui->skill1->currentIndex();
    currentPoke.skill_2 = ui->skill2->currentIndex();
    currentPoke.skill_3 = ui->skill3->currentIndex();
    currentPoke.skill_4 = ui->skill4->currentIndex();
    VBA.WriteValue_2(poke[currentPoke.index].skill_1,&currentPoke.skill_1);
    VBA.WriteValue_2(poke[currentPoke.index].skill_2,&currentPoke.skill_2);
    VBA.WriteValue_2(poke[currentPoke.index].skill_3,&currentPoke.skill_3);
    VBA.WriteValue_2(poke[currentPoke.index].skill_4,&currentPoke.skill_4);
}

void Widget::on_LeagueMode_clicked()
{
    u8 max = 99;
    for(int i=1;i<=6;i++)
    {
        if(VBA.ReadValue_4(poke[i].breedOffset) == 0)
            break;
        VBA.WriteValue_1(poke[i].PP1,&max);
        VBA.WriteValue_1(poke[i].PP2,&max);
        VBA.WriteValue_1(poke[i].PP3,&max);
        VBA.WriteValue_1(poke[i].PP4,&max);
    }
    on_refresh_clicked();
}

void Widget::on_MaxPointUp_clicked()
{
    u8 max = 0xFF;
    VBA.WriteValue_1(poke[currentPoke.index].PointUp,&max);
    on_refresh_clicked();
}

void Widget::on_MaxBeauty_clicked()
{
    ui->b1->setText("255");ui->b2->setText("255");ui->b3->setText("255");
    ui->b4->setText("255");ui->b5->setText("255");ui->b6->setText("0");
    writeBeauty();
}

void Widget::on_MaxID_clicked()
{
    ui->ab_hp->setText("31");ui->ab_atk->setText("31");ui->ab_def->setText("31");
    ui->ab_sp->setText("31");ui->ab_spatk->setText("31");ui->ab_spdef->setText("31");
    writeId();
}

void Widget::on_toEgg_clicked()
{
    currentPoke.ability = VBA.ReadValue_4(poke[currentPoke.index].abiliOffset);
    QString str = QString::number(currentPoke.ability,2);
    while(str.length() < 32)
        str.insert(0,'0');//获取个体值
    if(str[1] == '0')
        str[1] = '1';
    else if(str[1] == '1')
        str[1] = '0';
    currentPoke.ability = str.toUInt(nullptr,2);
    VBA.WriteValue_4(poke[currentPoke.index].abiliOffset,&currentPoke.ability);
}

void Widget::on_changeId_clicked()
{
    currentPoke.ability = VBA.ReadValue_4(poke[currentPoke.index].abiliOffset);
    QString str = QString::number(currentPoke.ability,2);
    while(str.length() < 32)
        str.insert(0,'0');//获取个体值
    if(str[0] == '0')
        str[0] = '1';
    else if(str[0] == '1')
        str[0] = '0';
    currentPoke.ability = str.toUInt(nullptr,2);
    VBA.WriteValue_4(poke[currentPoke.index].abiliOffset,&currentPoke.ability);
}

void Widget::on_changeHideId_clicked()
{
    currentPoke.hideID = VBA.ReadValue_2(poke[currentPoke.index].HideID);;
    QString str = QString::number(currentPoke.hideID,2);
    while(str.length()<16)
        str.insert(0,"0");
    if(ui->changeHideId->isChecked())//保留前15位
        str[15] = '1';
    else
        str[15] = '0';
    currentPoke.hideID = str.toUShort(nullptr,2);
    VBA.WriteValue_2(poke[currentPoke.index].HideID,&currentPoke.hideID);
}

void Widget::on_ribbon20_clicked()
{
    on_ribbon0_clicked();
    ui->ribbon_b1_4->setChecked(true);
    ui->ribbon_b2_4->setChecked(true);
    ui->ribbon_b3_4->setChecked(true);
    ui->ribbon_b4_4->setChecked(true);
    ui->ribbon_b5_4->setChecked(true);
}

void Widget::on_ribbon25_clicked()
{
    on_ribbon20_clicked();
    ui->ribbon_other_1->setChecked(true);
    ui->ribbon_other_2->setChecked(true);
    ui->ribbon_other_3->setChecked(true);
    ui->ribbon_other_4->setChecked(true);
    ui->ribbon_other_5->setChecked(true);
}

void Widget::on_ribbon32_clicked()
{
    on_ribbon25_clicked();
    ui->ribbon_unk_1->setChecked(true);
    ui->ribbon_unk_2->setChecked(true);
    ui->ribbon_unk_3->setChecked(true);
    ui->ribbon_unk_4->setChecked(true);
    ui->ribbon_unk_5->setChecked(true);
    ui->ribbon_unk_6->setChecked(true);
    ui->ribbon_unk_7->setChecked(true);
}

void Widget::on_ribbon0_clicked()
{
    ui->ribbon_b1_0->setChecked(true);
    ui->ribbon_b2_0->setChecked(true);
    ui->ribbon_b3_0->setChecked(true);
    ui->ribbon_b4_0->setChecked(true);
    ui->ribbon_b5_0->setChecked(true);
    ui->ribbon_other_1->setChecked(false);
    ui->ribbon_other_2->setChecked(false);
    ui->ribbon_other_3->setChecked(false);
    ui->ribbon_other_4->setChecked(false);
    ui->ribbon_other_5->setChecked(false);
    ui->ribbon_unk_1->setChecked(false);
    ui->ribbon_unk_2->setChecked(false);
    ui->ribbon_unk_3->setChecked(false);
    ui->ribbon_unk_4->setChecked(false);
    ui->ribbon_unk_5->setChecked(false);
    ui->ribbon_unk_6->setChecked(false);
    ui->ribbon_unk_7->setChecked(false);
}

void Widget::on_copy_clicked()
{
    u32 baseAddr;
    ReadProcessMemory(VBA.m_hProcess, (LPVOID)VBA.baseAddress, &baseAddr, 4, NULL);
    baseAddr += poke[currentPoke.index].begin;
    ReadProcessMemory(VBA.m_hProcess, (LPVOID)baseAddr, snipaste, 100, NULL);
    ui->labelSnipaste->setText(breed[currentPoke.breed].name);
    for(int i=0;i<100;i++)
        printf("%d,",snipaste[i]);
}

void Widget::on_paste_clicked()
{
    u32 baseAddr;
    ReadProcessMemory(VBA.m_hProcess, (LPVOID)VBA.baseAddress, &baseAddr, 4, NULL);
    baseAddr += poke[currentPoke.index].begin;
    WriteProcessMemory(VBA.m_hProcess, (LPVOID)baseAddr,snipaste,100,NULL);
    on_refresh_clicked();
}

void Widget::on_Tip_clicked()
{
    QDialog dlg(this);
    QLabel l(&dlg);
    dlg.setFixedSize(360,235);
    dlg.setWindowTitle(tr("捐助"));
    dlg.setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    l.setPixmap(QPixmap("://images/tip.png"));
    dlg.exec();
}

void Widget::on_comboPers_currentIndexChanged(int index)
{
    QString front = ui->linePersValue->text().mid(0,4);
    QString rear = ui->linePersValue->text().mid(4,4);
    u16 f = 0;
    u16 r = rear.toUShort(nullptr,16);
    u32 v = 0;
    //两段for,遍历全部65536
    for(int i=front.toInt(nullptr,16);i<=65535;i++)
    {
        f = (u16)i;
        v = f << 16;
        v += r;
        if(v%25 == (u32)index)
        {
            QString str = QString::number(v,16);
            while(str.length()<8)str.insert(0,"0");
            str = str.toUpper();
            ui->linePersValue->setText(str);
            return;
        }
    }
    for(int i=front.toInt(nullptr,16);i>=0;i--)
    {
        f = (u16)i;
        v = f << 16;
        v += r;
        if(v%25 == (u32)index)
        {
            QString str = QString::number(v,16);
            while(str.length()<8)str.insert(0,"0");
            str = str.toUpper();
            ui->linePersValue->setText(str);
            return;
        }
    }
}

void Widget::on_comboSex_currentIndexChanged(int index)
{
    QString front = ui->linePersValue->text().mid(0,4);
    QString r1 = ui->linePersValue->text().mid(4,2);
    QString r2 = ui->linePersValue->text().mid(6,2);
    u16 f = front.toUShort(nullptr,16);u16 r = 0;
    u8 ur1 = (u8)r1.toInt(nullptr,16),ur2 = 0;
    u32 v = 0;
    switch (index)
    {
    case 0://公
        for(int i=r2.toInt(nullptr,16);i<255;i++)
        {
            ur2 = (u8)i;
            if(ur2 >= currentPoke.sexRatio)//性别符合
            {
                r = ur1 << 8;r += ur2;
                v = f << 16;v += r;
                if(v%25 == (u32)ui->comboPers->currentIndex())//性格符合
                {
                    QString str = QString::number(v,16);
                    while(str.length()<8)str.insert(0,"0");
                    str = str.toUpper();
                    ui->linePersValue->setText(str);
                    return;
                }
            }
        }
        for(int i=r2.toInt(nullptr,16);i>=0;i--)
        {
            ur2 = (u8)i;
            if(ur2 >= currentPoke.sexRatio)//性别符合
            {
                r = ur1 << 8;r += ur2;
                v = f << 16;v += r;
                if(v%25 == (u32)ui->comboPers->currentIndex())//性格符合
                {
                    QString str = QString::number(v,16);
                    while(str.length()<8)str.insert(0,"0");
                    str = str.toUpper();
                    ui->linePersValue->setText(str);
                    return;
                }
            }
        }
        break;
    case 1://母
        for(int i=r2.toInt(nullptr,16);i<255;i++)
        {
            ur2 = (u8)i;
            if(ur2 < currentPoke.sexRatio)//性别符合
            {
                r = ur1 << 8;r += ur2;
                v = f << 16;v += r;
                if(v%25 == (u32)ui->comboPers->currentIndex())//性格符合
                {
                    QString str = QString::number(v,16);
                    while(str.length()<8)str.insert(0,"0");
                    str = str.toUpper();
                    ui->linePersValue->setText(str);
                    return;
                }
            }
        }
        for(int i=r2.toInt(nullptr,16);i>=0;i--)
        {
            ur2 = (u8)i;
            if(ur2 < currentPoke.sexRatio)//性别符合
            {
                r = ur1 << 8;r += ur2;
                v = f << 16;v += r;
                if(v%25 == (u32)ui->comboPers->currentIndex())//性格符合
                {
                    QString str = QString::number(v,16);
                    while(str.length()<8)str.insert(0,"0");
                    str = str.toUpper();
                    ui->linePersValue->setText(str);
                    return;
                }
            }
        }
    }
}

void Widget::on_switch_snipaste_clicked()
{
    BYTE snipaste1[100] = {213,1,213,1,0,0,0,0,9,102,5,80,255,100,0,3,112,21,2,2,14,44,14,234,255,255,255,0,0,0,0,0,151,0,1,0,20,44,16,0,255,255,0,0,19,0,127,0,57,0,15,0,24,24,24,48,0,252,0,252,0,6,0,0,0,0,0,0,15,201,30,235,255,255,255,63,36,201,255,135,0,0,0,0,100,255,85,1,85,1,72,1,236,0,43,1,212,0,237,0};
    BYTE snipaste2[100] = {213,1,213,1,0,0,0,0,9,102,5,80,255,100,0,3,112,21,2,2,14,44,14,234,255,255,255,0,0,0,0,0,151,0,1,0,20,44,16,0,255,255,0,0,70,0,148,0,249,0,35,1,24,32,24,16,0,252,0,252,0,6,0,0,0,0,0,0,15,201,30,235,255,255,255,63,36,201,255,135,0,0,0,0,100,255,85,1,85,1,72,1,236,0,43,1,212,0,237,0};
    QString str = tr("默认1:闪光梦幻");
    if(ui->labelSnipaste->text() != str)//切换默认1
    {
        ui->labelSnipaste->setText(str);
        memcpy(snipaste,snipaste1,100);
    }
    else//切换默认2
    {
        ui->labelSnipaste->setText(tr("默认2:闪光梦幻"));
        memcpy(snipaste,snipaste2,100);
    }
}

void Widget::on_stepsTo0_clicked()
{
    u8 zero = 0;
    VBA.WriteValue_1(poke[currentPoke.index].intiOffset,&zero);
    QMessageBox::about(this,tr("提示"),tr("即将孵化！\n请在地图上随意走动4s左右！"));
}

void Widget::on_deletePokemon_clicked()
{
    BYTE deletePM[100] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    u32 baseAddr;
    ReadProcessMemory(VBA.m_hProcess, (LPVOID)VBA.baseAddress, &baseAddr, 4, NULL);
    baseAddr += poke[currentPoke.index].begin;
    WriteProcessMemory(VBA.m_hProcess, (LPVOID)baseAddr,deletePM,100,NULL);
    on_refresh_clicked();
}

void Widget::on_toTieba_clicked()
{
    const QUrl regUrl(QLatin1String("https://tieba.baidu.com/f?kw=%E7%A9%B6%E6%9E%81%E7%BB%BF%E5%AE%9D%E7%9F%B3&fr=index"));
    QDesktopServices::openUrl(regUrl);
}

void Widget::on_enableSettings_clicked()
{
    lockFile.open(QIODevice::ReadOnly);
    QString startDialog(lockFile.readLine());startDialog.chop(2);   //开始对话框
    int tryNum = QString(lockFile.readLine()).toInt();              //试用次数
    QString cpuid(lockFile.readLine());cpuid.chop(2);               //机器码
    QString key = QString(lockFile.readLine());                     //激活码
    lockFile.close();

    lockFile.open(QIODevice::WriteOnly);
    if(ui->noStartDialog->isChecked())
        lockFile.write("1\r\n");
    else
        lockFile.write("0\r\n");
    lockFile.write(QString("%1\r\n").arg(tryNum).toUtf8().data());
    lockFile.write(QString(cpuid + "\r\n").toUtf8().data());
    lockFile.write(key.toUtf8().data());
    lockFile.close();
}

void Widget::on_battleRefresh_s_clicked()
{
    if(VBA.Attach())
    {
        qDebug() << "VBA的pid " << VBA.m_pid;
    }
    else
    {
        QMessageBox::critical(this,"MyPokeHack",tr("无法访问VBA的内存，\n"
                                               "可能是VBA版本不正确，\n"
                                               "或者游戏根本就没打开！"),QMessageBox::Ok);
        exit(1);
    }
    ui->ownCurrHP->setText(QString::number(VBA.ReadValue_2(0x240AC)));
    ui->ownAtk->setText(QString::number(VBA.ReadValue_2(0x24086)));
    ui->ownDef->setText(QString::number(VBA.ReadValue_2(0x24088)));
    ui->ownSpAtk->setText(QString::number(VBA.ReadValue_2(0x2408C)));
    ui->ownSpDef->setText(QString::number(VBA.ReadValue_2(0x2408E)));
    ui->ownSpeed->setText(QString::number(VBA.ReadValue_2(0x2408A)));
    ui->enemyCurrHP->setText(QString::number(VBA.ReadValue_2(0x24104)));
    ui->enemyAtk->setText(QString::number(VBA.ReadValue_2(0x240DE)));
    ui->enemyDef->setText(QString::number(VBA.ReadValue_2(0x240E0)));
    ui->enemySpAtk->setText(QString::number(VBA.ReadValue_2(0x240E4)));
    ui->enemySpDef->setText(QString::number(VBA.ReadValue_2(0x240E6)));
    ui->enemySpeed->setText(QString::number(VBA.ReadValue_2(0x240E2)));
}

void Widget::on_correction_clicked()
{
    on_refresh_clicked();
    //HP
    if(currentPoke.v[0] == 1) ui->hp->setText("1");
    else
    {
        currentPoke.totalHp = (currentPoke.v[0]*2 + ui->ab_hp->text().toInt() + ui->end_hp->text().toInt()/4) * ui->comboLevel->currentIndex() / 100 + 10 + ui->comboLevel->currentIndex();
        ui->hp->setText(QString::number(currentPoke.totalHp));
    }
    //非HP
    float atk_amend=1.0;float def_amend=1.0;float sp_amend=1.0;float spatk_amend=1.0;float spdef_amend=1.0;
    int pers = ui->comboPers->currentIndex();
    if(pers>=0 && pers <=4) atk_amend += 0.1;if(pers==0||pers==5||pers==10||pers==15||pers==20) atk_amend -= 0.1;
    if(pers>=5 && pers <=9) def_amend += 0.1;if(pers==1||pers==6||pers==11||pers==16||pers==21) def_amend -= 0.1;
    if(pers>=10 && pers <=14) sp_amend += 0.1;if(pers==2||pers==7||pers==12||pers==17||pers==22) sp_amend -= 0.1;
    if(pers>=15 && pers <=19) spatk_amend += 0.1;if(pers==3||pers==8||pers==13||pers==18||pers==23) spatk_amend -= 0.1;
    if(pers>=20 && pers <=24) spdef_amend += 0.1;if(pers==4||pers==9||pers==14||pers==19||pers==24) spdef_amend -= 0.1;
    currentPoke.Atk = (int)(((currentPoke.v[1]*2 + ui->ab_atk->text().toInt() + ui->end_atk->text().toInt()/4) * ui->comboLevel->currentIndex() / 100 + 5) * atk_amend);
    currentPoke.Def = (int)(((currentPoke.v[2]*2 + ui->ab_def->text().toInt() + ui->end_def->text().toInt()/4) * ui->comboLevel->currentIndex() / 100 + 5) * def_amend);
    currentPoke.Speed = (int)(((currentPoke.v[3]*2 + ui->ab_sp->text().toInt() + ui->end_sp->text().toInt()/4) * ui->comboLevel->currentIndex() / 100 + 5) * sp_amend);
    currentPoke.SpAtk = (int)(((currentPoke.v[4]*2 + ui->ab_spatk->text().toInt() + ui->end_spatk->text().toInt()/4) * ui->comboLevel->currentIndex() / 100 + 5) * spatk_amend);
    currentPoke.SpDef = (int)(((currentPoke.v[5]*2 + ui->ab_spdef->text().toInt() + ui->end_spdef->text().toInt()/4) * ui->comboLevel->currentIndex() / 100 + 5) * spdef_amend);
    ui->atk->setText(QString::number(currentPoke.Atk));
    ui->def->setText(QString::number(currentPoke.Def));
    ui->sp->setText(QString::number(currentPoke.Speed));
    ui->spatk->setText(QString::number(currentPoke.SpAtk));
    ui->spdef->setText(QString::number(currentPoke.SpDef));
    VBA.WriteValue_2(poke[currentPoke.index].totalHP,&currentPoke.totalHp);
    VBA.WriteValue_2(poke[currentPoke.index].Atk,&currentPoke.Atk);
    VBA.WriteValue_2(poke[currentPoke.index].Def,&currentPoke.Def);
    VBA.WriteValue_2(poke[currentPoke.index].Speed,&currentPoke.Speed);
    VBA.WriteValue_2(poke[currentPoke.index].SpAtk,&currentPoke.SpAtk);
    VBA.WriteValue_2(poke[currentPoke.index].SpDef,&currentPoke.SpDef);
}

void Widget::on_tabWidget_currentChanged(int index)
{
    switch (index) {
    case 0://精灵
        ui->refresh->setFocus();
        break;
    case 1://角色
        ui->refreshPlayer->setFocus();
        break;
    case 2://背包
        ui->refreshItem->setFocus();
        if(!readedFile)
        {
            readFile();
            readedFile = true;
        }
        if(addedPlayerItem)
            return;
        for(int i=0;i<ITEMNUM;i++)
        {
            QString str = QString::number(i,16);while(str.length()<3)str.insert(0,"0");str = str.toUpper();
            ui->comboPlayerItem->addItem(str + ": " + item[i].name);
        }
        addedPlayerItem = true;
        break;
    case 3://对战
        ui->battleRefresh_s->setFocus();
        break;
    case 4://关于
        ui->Tip->setFocus();
        break;
    }
}

void Widget::on_lockHP_s_clicked()
{
    if(ui->lockHP_s->isChecked())
    {
        lockedHP = ui->ownCurrHP->text().toUShort();
        timer_lockHp_s = new QTimer(this);
        connect(timer_lockHp_s,&QTimer::timeout,[=](){
            VBA.WriteValue_2(0x240AC,&lockedHP);
        });
        timer_lockHp_s->start(100);
    }
    else
    {
        timer_lockHp_s->stop();
        delete timer_lockHp_s;
        timer_lockHp_s = NULL;
    }
}

void Widget::on_lineNameInput_returnPressed()
{
    on_searchName_clicked();
}

void Widget::on_lineNameInput_textChanged(const QString &text)
{
    int size = 0;
    int l = text.length();
    for(int i=0;i<l;i++)
    {
        u16 uni = text.at(i).unicode();
        if(uni >= 0x4E00 && uni <= 0x9FA5)//这个字是中文
            size += 2;
        else
            size += 1;
    }
    if(size>7)
    {
        ui->lineNameInputWarning->setText(tr("名字长度不能超过7字节"));
        ui->searchName->setEnabled(false);
    }
    else
    {
        ui->lineNameInputWarning->setText("");
        ui->searchName->setEnabled(true);
    }
}

void Widget::on_searchName_clicked()
{
    if(! ui->searchName->isEnabled())
        return;
    QString text = ui->lineNameInput->text();
    int l = text.length();
    uncodef();
    QString str("");
    for(int i=0;i<l;i++)
    {
        int k = 0;
        for(int j=0;j<UNCODENUM;j++)
        {
            if(text.at(i) == uncode[j].Char)
            {
                str += (uncode[j].uni + " ");
                k++;
                break;
            }
        }
        if(!k)
            return;
    }
    if(str == "")
    {
        ui->linePlayerName->setText("FF FF FF FF FF FF FF");
        return;
    }
    str.chop(1);
    while(str.length() < 20)
    {
        str.append(" FF");
    }
    ui->linePlayerName->setText(str);
}

void Widget::on_writeItem_clicked()
{
    u16 item = ui->comboPlayerItem->currentIndex();
    u16 quantity = ui->linePlayereItemQuantity->text().toUShort();
    if(quantity == 0) quantity = 1;
    if(VBA.Attach())
    {
        qDebug() << "VBA的pid " << VBA.m_pid;
    }
    else
    {
        QMessageBox::critical(this,"MyPokeHack",tr("无法访问VBA的内存，\n"
                                               "可能是VBA版本不正确，\n"
                                               "或者游戏根本就没打开！"),QMessageBox::Ok);
        exit(1);
    }
    //判断是什么类型的道具
    u8 itemtype = VBA.ReadValue_1(0x00FC2C7C + item*44 + 26,VBA.ROMAddress);
    u32 offset = 0x3D030;int capacity = 79;
    int version = ui->comboVersion->currentIndex();
    if(version == 0)
        switch (itemtype) {
            case 1: offset = 0x3D030;capacity = 99;break;
            case 2: offset = 0x3D288;capacity = 31;break;
            case 3: offset = 0x3D308;capacity = 129;break;
            case 4: offset = 0x3D510;capacity = 49;break;
            case 5: offset = 0x3D1C0;capacity = 49;break;
            default: offset = 0x3D030;break;
        };
    if(version == 1)
        switch (itemtype) {
            case 1: offset = 0x3D030;capacity = 79;break;
            case 2: offset = 0x3D210;capacity = 23;break;
            case 3: offset = 0x3D270;capacity = 107;break;
            case 4: offset = 0x3D420;capacity = 49;break;
            case 5: offset = 0x3D170;capacity = 39;break;
            default: offset = 0x3D030;break;
        };
    //从第一个开始找，找到空位就添加，找不到替换掉最后一个
    for(int i=1;i<=capacity;i++)
    {
        if(VBA.ReadValue_2(offset) == 0)//找到空位，标记offset
            break;
        offset += 4;
    }
    //获取数量
    u16 Xor = VBA.PtrRead_2(0x000000AC,0x03005D90);
    quantity ^= Xor;
    VBA.WriteValue_2(offset,&item);
    VBA.WriteValue_2(offset + 2,&quantity);
    on_refreshItem_clicked();
}

void Widget::on_nameInput_textChanged(const QString &text)
{
    int size = 0;
    int l = text.length();
    for(int i=0;i<l;i++)
    {
        u16 uni = text.at(i).unicode();
        if(uni >= 0x4E00 && uni <= 0x9FA5)//这个字是中文
            size += 2;
        else
            size += 1;
    }
    if(size>10)
        ui->searchPokeName->setEnabled(false);
    else
        ui->searchPokeName->setEnabled(true);
}

void Widget::on_lineOTNameInput_textChanged(const QString &text)
{
    int size = 0;
    int l = text.length();
    for(int i=0;i<l;i++)
    {
        u16 uni = text.at(i).unicode();
        if(uni >= 0x4E00 && uni <= 0x9FA5)//这个字是中文
            size += 2;
        else
            size += 1;
    }
    if(size>7)
        ui->searchOTName->setEnabled(false);
    else
        ui->searchOTName->setEnabled(true);
}

void Widget::on_searchPokeName_clicked()
{
    if(! ui->searchPokeName->isEnabled())
        return;
    QString text = ui->nameInput->text();
    int l = text.length();
    uncodef();
    QString str("");
    for(int i=0;i<l;i++)
    {
        int k = 0;
        for(int j=0;j<UNCODENUM;j++)
        {
            if(text.at(i) == uncode[j].Char)
            {
                str += (uncode[j].uni + " ");
                k++;
                break;
            }
        }
        if(!k)
            return;
    }
    if(str == "")
    {
        ui->name->setText("FF FF FF FF FF FF FF FF FF FF");
        return;
    }
    str.chop(1);
    while(str.length() < 29)
    {
        str.append(" FF");
    }
    ui->name->setText(str);
}

void Widget::on_searchOTName_clicked()
{
    if(! ui->searchOTName->isEnabled())
        return;
    QString text = ui->lineOTNameInput->text();
    int l = text.length();
    uncodef();
    QString str("");
    for(int i=0;i<l;i++)
    {
        int k = 0;
        for(int j=0;j<UNCODENUM;j++)
        {
            if(text.at(i) == uncode[j].Char)
            {
                str += (uncode[j].uni + " ");
                k++;
                break;
            }
        }
        if(!k)
            return;
    }
    if(str == "")
    {
        ui->lineOTName->setText("FF FF FF FF FF FF FF");
        return;
    }
    str.chop(1);
    while(str.length() < 20)
    {
        str.append(" FF");
    }
    ui->lineOTName->setText(str);
}

void Widget::on_nameInput_returnPressed()
{
    on_searchPokeName_clicked();
}

void Widget::on_lineOTNameInput_returnPressed()
{
    on_searchOTName_clicked();
}

void Widget::on_writePokeName_clicked()
{
    QString str = ui->name->text();
    currentPoke.name[0] = str.mid(0,2).toUShort(nullptr,16);
    currentPoke.name[1] = str.mid(3,2).toUShort(nullptr,16);
    currentPoke.name[2] = str.mid(6,2).toUShort(nullptr,16);
    currentPoke.name[3] = str.mid(9,2).toUShort(nullptr,16);
    currentPoke.name[4] = str.mid(12,2).toUShort(nullptr,16);
    currentPoke.name[5] = str.mid(15,2).toUShort(nullptr,16);
    currentPoke.name[6] = str.mid(18,2).toUShort(nullptr,16);
    currentPoke.name[7] = str.mid(21,2).toUShort(nullptr,16);
    currentPoke.name[8] = str.mid(24,2).toUShort(nullptr,16);
    currentPoke.name[9] = str.mid(27,2).toUShort(nullptr,16);
    u32 address;//临时存放地址的变量
    ReadProcessMemory(VBA.m_hProcess, (LPVOID)0x005A8F50, &address, 4, NULL);//第一次读取
    address += (poke[currentPoke.index].begin + 8);//计算真实地址
    WriteProcessMemory(VBA.m_hProcess, (LPVOID)address, currentPoke.name, 10, NULL);
}

void Widget::on_writeOTName_clicked()
{
    QString str = ui->lineOTName->text();
    currentPoke.name[0] = str.mid(0,2).toUShort(nullptr,16);
    currentPoke.name[1] = str.mid(3,2).toUShort(nullptr,16);
    currentPoke.name[2] = str.mid(6,2).toUShort(nullptr,16);
    currentPoke.name[3] = str.mid(9,2).toUShort(nullptr,16);
    currentPoke.name[4] = str.mid(12,2).toUShort(nullptr,16);
    currentPoke.name[5] = str.mid(15,2).toUShort(nullptr,16);
    currentPoke.name[6] = str.mid(18,2).toUShort(nullptr,16);
    u32 address;//临时存放地址的变量
    ReadProcessMemory(VBA.m_hProcess, (LPVOID)0x005A8F50, &address, 4, NULL);//第一次读取
    address += (poke[currentPoke.index].begin + 20);//计算真实地址
    WriteProcessMemory(VBA.m_hProcess, (LPVOID)address, currentPoke.name, 7, NULL);
}

void Widget::on_refreshItem_clicked()
{
    if(VBA.Attach())
    {
        qDebug() << "VBA的pid " << VBA.m_pid;
    }
    else
    {
        QMessageBox::critical(this,"MyPokeHack",tr("无法访问VBA的内存，\n"
                                               "可能是VBA版本不正确，\n"
                                               "或者游戏根本就没打开！"),QMessageBox::Ok);
        exit(1);
    }
    ui->itemTable->clearContents();
    int version = ui->comboVersion->currentIndex();
    u32 addr1,addr2,addr3,addr4,addr5;
    u16 c1,c2,c3,c4,c5;
    if(version == 0)
    {
        addr1 = 0x3D030;addr2 = 0x3D288;addr3 = 0x3D308;addr4 = 0x3D510;addr5 = 0x3D1C0;
        c1 = 100;c2 = 32;c3 = 130;c4 = 50;c5 = 50;
    }
    else if(version == 1)
    {
        addr1 = 0x3D030;addr2 = 0x3D210;addr3 = 0x3D270;addr4 = 0x3D420;addr5 = 0x3D170;
        c1 = 80;c2 = 24;c3 = 108;c4 = 50;c5 = 40;
    }
    ui->itemTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    u16 Xor = VBA.PtrRead_2(0x000000AC,0x03005D90);
    //普通道具
    for(u16 i=0;i<c1;i++)
    {
        u16 index = VBA.ReadValue_2(addr1,0x005A8F50);
        u16 num = VBA.ReadValue_2(addr1+2,0x005A8F50);
        num ^= Xor;
        QString name = this->item[index].name;
        if(index != 0)
            name = this->item[index].name + "*" + QString::number(num);
        QTableWidgetItem *item = new QTableWidgetItem(name);
        ui->itemTable->setItem(i,0,item);
        addr1 += 4;
    }
    //精灵球
    for(u16 i=0;i<c2;i++)
    {
        u16 index = VBA.ReadValue_2(addr2,0x005A8F50);
        u16 num = VBA.ReadValue_2(addr2+2,0x005A8F50);
        num ^= Xor;
        QString name = this->item[index].name;
        if(index != 0)
            name = this->item[index].name + "*" + QString::number(num);
        QTableWidgetItem *item = new QTableWidgetItem(name);
        ui->itemTable->setItem(i,1,item);
        addr2 += 4;
    }
    //技能机
    for(u16 i=0;i<c3;i++)
    {
        u16 index = VBA.ReadValue_2(addr3,0x005A8F50);
        u16 num = VBA.ReadValue_2(addr3+2,0x005A8F50);
        num ^= Xor;
        QString name = this->item[index].name;
        if(index != 0)
            name = this->item[index].name + "*" + QString::number(num);
        QTableWidgetItem *item = new QTableWidgetItem(name);
        ui->itemTable->setItem(i,2,item);
        addr3 += 4;
    }
    //树果
    for(u16 i=0;i<c4;i++)
    {
        u16 index = VBA.ReadValue_2(addr4,0x005A8F50);
        u16 num = VBA.ReadValue_2(addr4+2,0x005A8F50);
        num ^= Xor;
        QString name = this->item[index].name;
        if(index != 0)
            name = this->item[index].name + "*" + QString::number(num);
        QTableWidgetItem *item = new QTableWidgetItem(name);
        ui->itemTable->setItem(i,3,item);
        addr4 += 4;
    }
    //重要道具
    for(u16 i=0;i<c5;i++)
    {
        u16 index = VBA.ReadValue_2(addr5,0x005A8F50);
        u16 num = VBA.ReadValue_2(addr5+2,0x005A8F50);
        num ^= Xor;
        QString name = this->item[index].name;
        if(index != 0)
            name = this->item[index].name + "*" + QString::number(num);
        QTableWidgetItem *item = new QTableWidgetItem(name);
        ui->itemTable->setItem(i,4,item);
        addr5 += 4;
    }
}

void Widget::on_oneHitKill_s_clicked()
{
    if(ui->oneHitKill_s->isChecked())
    {
        timer_oneHit_s = new QTimer(this);
        connect(timer_oneHit_s,&QTimer::timeout,[=](){
            u16 atk = 65535;
            u16 def = 1;
            VBA.WriteValue_2(0x24086,&atk);
            VBA.WriteValue_2(0x2408C,&atk);
            VBA.WriteValue_2(0x240E0,&def);
            VBA.WriteValue_2(0x240E6,&def);
        });
        timer_oneHit_s->start(100);
        ui->ownAtk->setText("65535");
        ui->ownSpAtk->setText("65535");
        ui->enemyDef->setText("1");
        ui->enemySpDef->setText("1");
    }
    else
    {
        timer_oneHit_s->stop();
        delete timer_oneHit_s;
        timer_oneHit_s = NULL;
    }
}

void Widget::on_battleDetect_s_clicked()
{
    if(ui->battleDetect_s->isChecked())
    {
        timer_BattDected_s = new QTimer(this);
        connect(timer_BattDected_s,&QTimer::timeout,[=](){
            ui->ownCurrHP->setText(QString::number(VBA.ReadValue_2(0x240AC)));
            ui->ownAtk->setText(QString::number(VBA.ReadValue_2(0x24086)));
            ui->ownDef->setText(QString::number(VBA.ReadValue_2(0x24088)));
            ui->ownSpAtk->setText(QString::number(VBA.ReadValue_2(0x2408C)));
            ui->ownSpDef->setText(QString::number(VBA.ReadValue_2(0x2408E)));
            ui->ownSpeed->setText(QString::number(VBA.ReadValue_2(0x2408A)));
            ui->enemyCurrHP->setText(QString::number(VBA.ReadValue_2(0x24104)));
            ui->enemyAtk->setText(QString::number(VBA.ReadValue_2(0x240DE)));
            ui->enemyDef->setText(QString::number(VBA.ReadValue_2(0x240E0)));
            ui->enemySpAtk->setText(QString::number(VBA.ReadValue_2(0x240E4)));
            ui->enemySpDef->setText(QString::number(VBA.ReadValue_2(0x240E6)));
            ui->enemySpeed->setText(QString::number(VBA.ReadValue_2(0x240E2)));
        });
        timer_BattDected_s->start(500);
    }
    else
    {
        timer_BattDected_s->stop();
        delete timer_BattDected_s;
        timer_BattDected_s = NULL;
    }
}

void Widget::on_battleWrite_s_clicked()
{
    u16 ohp = ui->ownCurrHP->text().toUShort();
    u16 oatk = ui->ownAtk->text().toUShort();
    u16 odef = ui->ownDef->text().toUShort();
    u16 ospeed = ui->ownSpeed->text().toUShort();
    u16 ospatk = ui->ownSpAtk->text().toUShort();
    u16 ospdef = ui->ownSpDef->text().toUShort();
    u16 ehp = ui->enemyCurrHP->text().toUShort();
    u16 eatk = ui->enemyAtk->text().toUShort();
    u16 edef = ui->enemyDef->text().toUShort();
    u16 espeed = ui->enemySpeed->text().toUShort();
    u16 espatk = ui->enemySpAtk->text().toUShort();
    u16 espdef = ui->enemySpDef->text().toUShort();
    VBA.WriteValue_2(0x240AC,&ohp);
    VBA.WriteValue_2(0x24086,&oatk);
    VBA.WriteValue_2(0x24088,&odef);
    VBA.WriteValue_2(0x2408A,&ospeed);
    VBA.WriteValue_2(0x2408C,&ospatk);
    VBA.WriteValue_2(0x2408E,&ospdef);
    VBA.WriteValue_2(0x24104,&ehp);
    VBA.WriteValue_2(0x240DE,&eatk);
    VBA.WriteValue_2(0x240E0,&edef);
    VBA.WriteValue_2(0x240E2,&espeed);
    VBA.WriteValue_2(0x240E4,&espatk);
    VBA.WriteValue_2(0x240E6,&espdef);
    on_battleRefresh_s_clicked();
}
void Widget::on_invincible_s_clicked()
{
    if(ui->invincible_s->isChecked())
    {
        ui->enemyAtk->setText("0");
        ui->enemySpAtk->setText("0");
        timer_invincible_s = new QTimer(this);
        connect(timer_invincible_s,&QTimer::timeout,[=](){
            u16 atk = 0;
            VBA.WriteValue_2(0x240DE,&atk);
            VBA.WriteValue_2(0x240E4,&atk);
        });
        timer_invincible_s->start(100);
    }
    else
    {
        timer_invincible_s->stop();
        delete timer_invincible_s;
        timer_invincible_s = NULL;
    }
}

void Widget::on_onekeyCloseLocking_s_clicked()
{
    if(timer_lockHp_s != NULL)
    {
        timer_lockHp_s->stop();
        delete timer_lockHp_s;
        timer_lockHp_s = NULL;
    }
    if(timer_oneHit_s != NULL)
    {
        timer_oneHit_s->stop();
        delete timer_oneHit_s;
        timer_oneHit_s = NULL;
    }
    if(timer_invincible_s != NULL)
    {
        timer_invincible_s->stop();
        delete timer_invincible_s;
        timer_invincible_s = NULL;
    }
    if(timer_BattDected_s != NULL)
    {
        timer_BattDected_s->stop();
        delete timer_BattDected_s;
        timer_BattDected_s = NULL;
    }
    ui->lockHP_s->setChecked(false);
    ui->oneHitKill_s->setChecked(false);
    ui->invincible_s->setChecked(false);
    ui->battleDetect_s->setChecked(false);
}

void Widget::on_getVBA_clicked()
{
    const QUrl regUrl(QLatin1String("https://pan.baidu.com/s/15tjglfC-pZXPBtqIIU46Ug"));
    QDesktopServices::openUrl(regUrl);
}

void Widget::on_battleRefresh_d_clicked()
{
    if(VBA.Attach())
    {
        qDebug() << "VBA的pid " << VBA.m_pid;
    }
    else
    {
        QMessageBox::critical(this,"MyPokeHack",tr("无法访问VBA的内存，\n"
                                               "可能是VBA版本不正确，\n"
                                               "或者游戏根本就没打开！"),QMessageBox::Ok);
        exit(1);
    }
    //己方左
    ui->ownCurrHP1->setText(QString::number(VBA.ReadValue_2(0x240AC)));
    ui->ownAtk1->setText(QString::number(VBA.ReadValue_2(0x24086)));
    ui->ownDef1->setText(QString::number(VBA.ReadValue_2(0x24088)));
    ui->ownSpAtk1->setText(QString::number(VBA.ReadValue_2(0x2408C)));
    ui->ownSpDef1->setText(QString::number(VBA.ReadValue_2(0x2408E)));
    ui->ownSpeed1->setText(QString::number(VBA.ReadValue_2(0x2408A)));
    //己方右
    ui->ownCurrHP2->setText(QString::number(VBA.ReadValue_2(0x2415C)));
    ui->ownAtk2->setText(QString::number(VBA.ReadValue_2(0x24136)));
    ui->ownDef2->setText(QString::number(VBA.ReadValue_2(0x24138)));
    ui->ownSpAtk2->setText(QString::number(VBA.ReadValue_2(0x2413C)));
    ui->ownSpDef2->setText(QString::number(VBA.ReadValue_2(0x2413E)));
    ui->ownSpeed2->setText(QString::number(VBA.ReadValue_2(0x2413A)));
    //敌方左
    ui->enemyCurrHP1->setText(QString::number(VBA.ReadValue_2(0x241B4)));
    ui->enemyAtk1->setText(QString::number(VBA.ReadValue_2(0x2418E)));
    ui->enemyDef1->setText(QString::number(VBA.ReadValue_2(0x24190)));
    ui->enemySpAtk1->setText(QString::number(VBA.ReadValue_2(0x24194)));
    ui->enemySpDef1->setText(QString::number(VBA.ReadValue_2(0x24196)));
    ui->enemySpeed1->setText(QString::number(VBA.ReadValue_2(0x24192)));
    //敌方右
    ui->enemyCurrHP2->setText(QString::number(VBA.ReadValue_2(0x24104)));
    ui->enemyAtk2->setText(QString::number(VBA.ReadValue_2(0x240DE)));
    ui->enemyDef2->setText(QString::number(VBA.ReadValue_2(0x240E0)));
    ui->enemySpAtk2->setText(QString::number(VBA.ReadValue_2(0x240E4)));
    ui->enemySpDef2->setText(QString::number(VBA.ReadValue_2(0x240E6)));
    ui->enemySpeed2->setText(QString::number(VBA.ReadValue_2(0x240E2)));
}

void Widget::on_battleDetect_d_clicked()
{
    if(ui->battleDetect_d->isChecked())
    {
        timer_BattDected_d = new QTimer(this);
        connect(timer_BattDected_d,&QTimer::timeout,[=](){
            ui->ownCurrHP1->setText(QString::number(VBA.ReadValue_2(0x240AC)));
            ui->ownAtk1->setText(QString::number(VBA.ReadValue_2(0x24086)));
            ui->ownDef1->setText(QString::number(VBA.ReadValue_2(0x24088)));
            ui->ownSpAtk1->setText(QString::number(VBA.ReadValue_2(0x2408C)));
            ui->ownSpDef1->setText(QString::number(VBA.ReadValue_2(0x2408E)));
            ui->ownSpeed1->setText(QString::number(VBA.ReadValue_2(0x2408A)));
            ui->ownCurrHP2->setText(QString::number(VBA.ReadValue_2(0x2415C)));
            ui->ownAtk2->setText(QString::number(VBA.ReadValue_2(0x24136)));
            ui->ownDef2->setText(QString::number(VBA.ReadValue_2(0x24138)));
            ui->ownSpAtk2->setText(QString::number(VBA.ReadValue_2(0x2413C)));
            ui->ownSpDef2->setText(QString::number(VBA.ReadValue_2(0x2413E)));
            ui->ownSpeed2->setText(QString::number(VBA.ReadValue_2(0x2413A)));
            ui->enemyCurrHP1->setText(QString::number(VBA.ReadValue_2(0x241B4)));
            ui->enemyAtk1->setText(QString::number(VBA.ReadValue_2(0x2418E)));
            ui->enemyDef1->setText(QString::number(VBA.ReadValue_2(0x24190)));
            ui->enemySpAtk1->setText(QString::number(VBA.ReadValue_2(0x24194)));
            ui->enemySpDef1->setText(QString::number(VBA.ReadValue_2(0x24196)));
            ui->enemySpeed1->setText(QString::number(VBA.ReadValue_2(0x24192)));
            ui->enemyCurrHP2->setText(QString::number(VBA.ReadValue_2(0x24104)));
            ui->enemyAtk2->setText(QString::number(VBA.ReadValue_2(0x240DE)));
            ui->enemyDef2->setText(QString::number(VBA.ReadValue_2(0x240E0)));
            ui->enemySpAtk2->setText(QString::number(VBA.ReadValue_2(0x240E4)));
            ui->enemySpDef2->setText(QString::number(VBA.ReadValue_2(0x240E6)));
            ui->enemySpeed2->setText(QString::number(VBA.ReadValue_2(0x240E2)));
        });
        timer_BattDected_d->start(500);
    }
    else
    {
        timer_BattDected_d->stop();
        delete timer_BattDected_d;
        timer_BattDected_d = NULL;
    }
}

void Widget::on_onekeyCloseLocking_d_clicked()
{
    if(timer_lockHp_d != NULL)
    {
        timer_lockHp_d->stop();
        delete timer_lockHp_d;
        timer_lockHp_d = NULL;
    }
    if(timer_oneHit_d != NULL)
    {
        timer_oneHit_d->stop();
        delete timer_oneHit_d;
        timer_oneHit_d = NULL;
    }
    if(timer_invincible_d != NULL)
    {
        timer_invincible_d->stop();
        delete timer_invincible_d;
        timer_invincible_d = NULL;
    }
    if(timer_BattDected_d != NULL)
    {
        timer_BattDected_d->stop();
        delete timer_BattDected_d;
        timer_BattDected_d = NULL;
    }
    ui->lockHP_d->setChecked(false);
    ui->oneHitKill_d->setChecked(false);
    ui->invincible_d->setChecked(false);
    ui->battleDetect_d->setChecked(false);
}

void Widget::on_battleWrite_d_clicked()
{
    u16 ohp1 = ui->ownCurrHP1->text().toUShort();
    u16 oatk1 = ui->ownAtk1->text().toUShort();
    u16 odef1 = ui->ownDef1->text().toUShort();
    u16 ospeed1 = ui->ownSpeed1->text().toUShort();
    u16 ospatk1 = ui->ownSpAtk1->text().toUShort();
    u16 ospdef1 = ui->ownSpDef1->text().toUShort();
    u16 ohp2 = ui->ownCurrHP2->text().toUShort();
    u16 oatk2 = ui->ownAtk2->text().toUShort();
    u16 odef2 = ui->ownDef2->text().toUShort();
    u16 ospeed2 = ui->ownSpeed2->text().toUShort();
    u16 ospatk2 = ui->ownSpAtk2->text().toUShort();
    u16 ospdef2 = ui->ownSpDef2->text().toUShort();
    u16 ehp1 = ui->enemyCurrHP1->text().toUShort();
    u16 eatk1 = ui->enemyAtk1->text().toUShort();
    u16 edef1 = ui->enemyDef1->text().toUShort();
    u16 espeed1 = ui->enemySpeed1->text().toUShort();
    u16 espatk1 = ui->enemySpAtk1->text().toUShort();
    u16 espdef1 = ui->enemySpDef1->text().toUShort();
    u16 ehp2 = ui->enemyCurrHP2->text().toUShort();
    u16 eatk2 = ui->enemyAtk2->text().toUShort();
    u16 edef2 = ui->enemyDef2->text().toUShort();
    u16 espeed2 = ui->enemySpeed2->text().toUShort();
    u16 espatk2 = ui->enemySpAtk2->text().toUShort();
    u16 espdef2 = ui->enemySpDef2->text().toUShort();
    VBA.WriteValue_2(0x240AC,&ohp1);
    VBA.WriteValue_2(0x24086,&oatk1);
    VBA.WriteValue_2(0x24088,&odef1);
    VBA.WriteValue_2(0x2408A,&ospeed1);
    VBA.WriteValue_2(0x2408C,&ospatk1);
    VBA.WriteValue_2(0x2408E,&ospdef1);
    VBA.WriteValue_2(0x2415C,&ohp2);
    VBA.WriteValue_2(0x24136,&oatk2);
    VBA.WriteValue_2(0x24138,&odef2);
    VBA.WriteValue_2(0x2413A,&ospeed2);
    VBA.WriteValue_2(0x2413C,&ospatk2);
    VBA.WriteValue_2(0x2413E,&ospdef2);
    VBA.WriteValue_2(0x241B4,&ehp1);
    VBA.WriteValue_2(0x2418E,&eatk1);
    VBA.WriteValue_2(0x24190,&edef1);
    VBA.WriteValue_2(0x24192,&espeed1);
    VBA.WriteValue_2(0x24194,&espatk1);
    VBA.WriteValue_2(0x24196,&espdef1);
    VBA.WriteValue_2(0x24104,&ehp2);
    VBA.WriteValue_2(0x240DE,&eatk2);
    VBA.WriteValue_2(0x240E0,&edef2);
    VBA.WriteValue_2(0x240E2,&espeed2);
    VBA.WriteValue_2(0x240E4,&espatk2);
    VBA.WriteValue_2(0x240E6,&espdef2);
    on_battleRefresh_d_clicked();
}

void Widget::on_lockHP_d_clicked()
{
    if(ui->lockHP_d->isChecked())
    {
        lockedHP = ui->ownCurrHP1->text().toUShort();
        lockedHP2 = ui->ownCurrHP2->text().toUShort();
        timer_lockHp_d = new QTimer(this);
        connect(timer_lockHp_d,&QTimer::timeout,[=](){
            VBA.WriteValue_2(0x240AC,&lockedHP);
            VBA.WriteValue_2(0x2415C,&lockedHP2);
        });
        timer_lockHp_d->start(100);
    }
    else
    {
        timer_lockHp_d->stop();
        delete timer_lockHp_d;
        timer_lockHp_d = NULL;
    }
}

void Widget::on_oneHitKill_d_clicked()
{
    if(ui->oneHitKill_d->isChecked())
    {
        timer_oneHit_d = new QTimer(this);
        connect(timer_oneHit_d,&QTimer::timeout,[=](){
            u16 atk = 65535;
            u16 def = 1;
            VBA.WriteValue_2(0x24086,&atk);
            VBA.WriteValue_2(0x2408C,&atk);
            VBA.WriteValue_2(0x240E0,&def);
            VBA.WriteValue_2(0x240E6,&def);
            VBA.WriteValue_2(0x24136,&atk);
            VBA.WriteValue_2(0x2413C,&atk);
            VBA.WriteValue_2(0x24190,&def);
            VBA.WriteValue_2(0x24196,&def);
        });
        timer_oneHit_d->start(100);
        ui->ownAtk1->setText("65535");
        ui->ownSpAtk1->setText("65535");
        ui->enemyDef1->setText("1");
        ui->enemySpDef1->setText("1");
        ui->ownAtk2->setText("65535");
        ui->ownSpAtk2->setText("65535");
        ui->enemyDef2->setText("1");
        ui->enemySpDef2->setText("1");
    }
    else
    {
        timer_oneHit_d->stop();
        delete timer_oneHit_d;
        timer_oneHit_d = NULL;
    }
}

void Widget::on_invincible_d_clicked()
{
    if(ui->invincible_d->isChecked())
    {
        ui->enemyAtk1->setText("0");
        ui->enemySpAtk1->setText("0");
        ui->enemyAtk2->setText("0");
        ui->enemySpAtk2->setText("0");
        timer_invincible_d = new QTimer(this);
        connect(timer_invincible_d,&QTimer::timeout,[=](){
            u16 atk = 0;
            VBA.WriteValue_2(0x240DE,&atk);
            VBA.WriteValue_2(0x240E4,&atk);
            VBA.WriteValue_2(0x2418E,&atk);
            VBA.WriteValue_2(0x24194,&atk);
        });
        timer_invincible_d->start(100);
    }
    else
    {
        timer_invincible_d->stop();
        delete timer_invincible_d;
        timer_invincible_d = NULL;
    }
}

void Widget::on_lockMega_clicked()
{
    /*
    if(ui->lockMega->isChecked())
    {
        ui->lockMega->setText(tr("还原状态(一次战斗后还原)"));
        poke[currentPoke.index].megaLocked = true;
        timer_lockMega = new QTimer(this);
        //监测该精灵有没有mega形态
        bool hasMega = false;
        poke[currentPoke.index].megaLockedNum = currentPoke.breed;

        QString sub = breed[currentPoke.breed].name;
        sub = "Mega" + sub;
//        qDebug() << sub;
        for(int i=0;i<BREEDNUM;i++)
        {
            if(breed[i].name.contains(sub))//有Mega形态
            {
                poke[currentPoke.index].megaLockedNum  = i;
                hasMega = true;
            }
        }
        if(!hasMega)
        {
            //提示没有Mega形态
            QMessageBox::warning(this,tr("提示"),tr("该宝可梦没有Mega形态"));
            //按钮还原
            ui->lockMega->setChecked(false);
            ui->lockMega->setText(tr("锁定Mega形态(存在Mega形态)"));
            poke[currentPoke.index].megaLocked = false;
            return;
        }
        connect(timer_lockMega,&QTimer::timeout,[=](){
            //进行锁定操作
            VBA.WriteValue_2(poke[currentPoke.index].breedOffset,&poke[currentPoke.index].megaLockedNum);
        });
        timer_lockMega->start(500);
//        readPoke(currentPoke.index);
//        on_correction_clicked();

        for (int i = 0; i < 6; i++)
            currentPoke.v[i] = VBA.ReadValue_1(0x00F186E0 + 28*poke[currentPoke.index].megaLockedNum + i,VBA.ROMAddress);
        currentPoke.totalHp = (currentPoke.v[0]*2 + ui->ab_hp->text().toInt() + ui->end_hp->text().toInt()/4) * ui->comboLevel->currentIndex() / 100 + 10 + ui->comboLevel->currentIndex();
        ui->hp->setText(QString::number(currentPoke.totalHp));
        float atk_amend=1.0;float def_amend=1.0;float sp_amend=1.0;float spatk_amend=1.0;float spdef_amend=1.0;
        int pers = ui->comboPers->currentIndex();
        if(pers>=0 && pers <=4) atk_amend += 0.1;if(pers==0||pers==5||pers==10||pers==15||pers==20) atk_amend -= 0.1;
        if(pers>=5 && pers <=9) def_amend += 0.1;if(pers==1||pers==6||pers==11||pers==16||pers==21) def_amend -= 0.1;
        if(pers>=10 && pers <=14) sp_amend += 0.1;if(pers==2||pers==7||pers==12||pers==17||pers==22) sp_amend -= 0.1;
        if(pers>=15 && pers <=19) spatk_amend += 0.1;if(pers==3||pers==8||pers==13||pers==18||pers==23) spatk_amend -= 0.1;
        if(pers>=20 && pers <=24) spdef_amend += 0.1;if(pers==4||pers==9||pers==14||pers==19||pers==24) spdef_amend -= 0.1;
        currentPoke.Atk = (int)(((currentPoke.v[1]*2 + ui->ab_atk->text().toInt() + ui->end_atk->text().toInt()/4) * ui->comboLevel->currentIndex() / 100 + 5) * atk_amend);
        currentPoke.Def = (int)(((currentPoke.v[2]*2 + ui->ab_def->text().toInt() + ui->end_def->text().toInt()/4) * ui->comboLevel->currentIndex() / 100 + 5) * def_amend);
        currentPoke.Speed = (int)(((currentPoke.v[3]*2 + ui->ab_sp->text().toInt() + ui->end_sp->text().toInt()/4) * ui->comboLevel->currentIndex() / 100 + 5) * sp_amend);
        currentPoke.SpAtk = (int)(((currentPoke.v[4]*2 + ui->ab_spatk->text().toInt() + ui->end_spatk->text().toInt()/4) * ui->comboLevel->currentIndex() / 100 + 5) * spatk_amend);
        currentPoke.SpDef = (int)(((currentPoke.v[5]*2 + ui->ab_spdef->text().toInt() + ui->end_spdef->text().toInt()/4) * ui->comboLevel->currentIndex() / 100 + 5) * spdef_amend);
        ui->atk->setText(QString::number(currentPoke.Atk));
        ui->def->setText(QString::number(currentPoke.Def));
        ui->sp->setText(QString::number(currentPoke.Speed));
        ui->spatk->setText(QString::number(currentPoke.SpAtk));
        ui->spdef->setText(QString::number(currentPoke.SpDef));
        VBA.WriteValue_2(poke[currentPoke.index].totalHP,&currentPoke.totalHp);
        VBA.WriteValue_2(poke[currentPoke.index].Atk,&currentPoke.Atk);
        VBA.WriteValue_2(poke[currentPoke.index].Def,&currentPoke.Def);
        VBA.WriteValue_2(poke[currentPoke.index].Speed,&currentPoke.Speed);
        VBA.WriteValue_2(poke[currentPoke.index].SpAtk,&currentPoke.SpAtk);
        VBA.WriteValue_2(poke[currentPoke.index].SpDef,&currentPoke.SpDef);
        qDebug() << tr("能力修正");
    }
    else
    {
        ui->lockMega->setText(tr("锁定Mega形态(存在Mega形态)"));
        poke[currentPoke.index].megaLocked = false;
        timer_lockMega->stop();
        delete timer_lockMega;
        timer_lockMega = NULL;
    }
    */
}

void Widget::on_itemTable_itemClicked(QTableWidgetItem *item)
{
    ui->labelcurrSelectItem->setText(item->text());
}

void Widget::on_deleteItem_clicked()
{
    QTableWidgetItem *item = ui->itemTable->currentItem();
    if(item->text() == tr("无"))
        return;
    u32 addr[6];//背包基址
    if(ui->comboVersion->currentIndex() == 0)//IV
    {
        addr[1] = 0x3D030;addr[2] = 0x3D288;addr[3] = 0x3D308;addr[4] = 0x3D510;addr[5] = 0x3D1C0;
    }
    else if(ui->comboVersion->currentIndex() == 1)//III
    {
        addr[1] = 0x3D030;addr[2] = 0x3D210;addr[3] = 0x3D270;addr[4] = 0x3D420;addr[5] = 0x3D170;
    }
    //计算是要删除哪一个背包
    int bag = item->column() + 1;
    addr[0] = addr[bag];
    //计算删除第几个道具
    int n = item->row() + 1;
    //将itemNum写成0，在将itemCount写成Xor
    u16 itemCount = 0;
    u16 Xor = VBA.PtrRead_2(0x000000AC,0x03005D90);
    VBA.WriteValue_2(addr[0] + 4*(n-1),&itemCount);
    VBA.WriteValue_2(addr[0]+2 + 4*(n-1),&Xor);
    //刷新一下背包，在游戏中切换一下背包会自动补齐
    on_refreshItem_clicked();
}

void Widget::on_EnableMirageIsland_clicked()
{
    //获取自己首位精灵的性格值低2位字节
    u32 island_32 = VBA.ReadValue_4(poke[1].PersOffset);
    u16 island_16 = island_32 & 0xffff;
    VBA.PtrWrite_2(0x00002390,&island_16);
    QMessageBox::about(this,tr("幻岛"),tr("已出现幻岛\n请不要将首位PM放回！"));
}

void Widget::on_getMCode_clicked()
{
    QString cpu_id = get_cpuId();
    QDialog dlg(this);
    dlg.setFixedSize(260,102);
    dlg.setWindowTitle(tr("机器码"));
    dlg.setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    QLabel text;
    text.setStyleSheet(""
                    "font: 10pt \"微软雅黑\";"
                    "font-size:12px;"
                    "");
    text.setGeometry(34,29,170,18);
    text.setText(tr("本机机器码："));
    text.setParent(&dlg);
    QLineEdit *edit1 = new QLineEdit(&dlg);
    edit1->setGeometry(110,29,120,18);
    edit1->setReadOnly(true);
    edit1->setText(cpu_id);

    QPushButton *btnCopy = new QPushButton(tr("复制机器码"));
    btnCopy->setGeometry(70,60,75,25);
    btnCopy->setParent(&dlg);

    QLabel *tip = new QLabel(&dlg);
    tip->setStyleSheet(""
                       "font: 10pt \"微软雅黑\";"
                       "font-size:12px;"
                       "");
    tip->setGeometry(160,63,110,18);

    QTimer *t = new QTimer(&dlg);
    connect(t,&QTimer::timeout,[=](){
        tip->setText("");
    });

    connect(btnCopy,&QPushButton::clicked,[=](){
        QClipboard *clipboard = QApplication::clipboard();//获取系统剪贴板指针
        clipboard->setText(cpu_id);
        tip->setText(tr("已复制到剪贴板"));
        t->start(1000);
    });

    dlg.exec();
    delete edit1;
    delete btnCopy;
    delete tip;
    delete t;
}

void Widget::on_normalItemTo99_clicked()
{
    u32 offset = 0x3D030;int capacity = 79;
    int version = ui->comboVersion->currentIndex();
    if(version == 0)
        capacity = 99;
    if(version == 1)
        capacity = 79;
    //获取数量
    u16 max = 99;
    u16 Xor = VBA.PtrRead_2(0x000000AC,0x03005D90);
    u16 quantity = max ^ Xor;

    for(int i=0;i<capacity;i++)
    {
        //如果道具不为空，就把数量改成99
        if(0 == VBA.ReadValue_2(offset))
            break;
        VBA.WriteValue_2(offset+2,&quantity);
        offset += 4;
    }
    on_refreshItem_clicked();
}

void Widget::on_getMegaVersion_clicked()
{
    QMessageBox::about(this, tr("永久Mega"), tr("请在弹出的链接中下载永久mega版本\n点击OK后弹出"));
    const QUrl regUrl(QLatin1String("https://www.lanzous.com/b653829"));
    QDesktopServices::openUrl(regUrl);
}

void Widget::on_allSkillItem_clicked()
{
    u32 offset = 0x3D270;
    int version = ui->comboVersion->currentIndex();
    //根据版本确定偏移
    if(version == 0)
        offset = 0x3D308;
    if(version == 1)
        offset = 0x3D270;

    //技能机器是连续的108个，其首编号(磨爪)为 0x17A
    u16 no = 0x17A;
    u16 Xor = VBA.PtrRead_2(0x000000AC,0x03005D90);
    u16 quantity = 1;
    quantity ^= Xor;
    for(int i=0;i<108;i++)
    {
        //写入道具
        VBA.WriteValue_2(offset,&no);
        no++;
        //写入数量
        VBA.WriteValue_2(offset+2,&quantity);
        offset += 4;
    }
    on_refreshItem_clicked();
}

void Widget::on_fruitTo99_clicked()
{
    u32 offset = 0x3D420;
    int version = ui->comboVersion->currentIndex();
    //根据版本确定偏移
    if(version == 0)
        offset = 0x3D510;
    if(version == 1)
        offset = 0x3D420;

    //树果是连续的43个，其首编号为 0x085
    u16 no = 0x085;
    u16 Xor = VBA.PtrRead_2(0x000000AC,0x03005D90);
    u16 quantity = 99;
    quantity ^= Xor;
    for(int i=0;i<43;i++)
    {
        //写入道具
        VBA.WriteValue_2(offset,&no);
        no++;
        //写入数量
        VBA.WriteValue_2(offset+2,&quantity);
        offset += 4;
    }
    on_refreshItem_clicked();
}

void Widget::on_ballTo99_clicked()
{
    u32 offset = 0x3D210;
    int version = ui->comboVersion->currentIndex();
    //根据版本确定偏移
    if(version == 0)
        offset = 0x3D288;
    if(version == 1)
        offset = 0x3D210;
    u16 arr[24];
    arr[0] = 0x001;

    for(int i=1,j=0x005;j<=0x00C;i++,j++)
    {
        arr[i] = j;
    }
    for(int i=9,j=0x121;j<=0x12F;i++,j++)
    {
        arr[i] = j;
    }

    u16 Xor = VBA.PtrRead_2(0x000000AC,0x03005D90);
    u16 quantity = 99;
    quantity ^= Xor;
    for(int i=0;i<24;i++)
    {
        //写入道具
        VBA.WriteValue_2(offset,arr+i);
        //写入数量
        VBA.WriteValue_2(offset+2,&quantity);
        offset += 4;
    }
    on_refreshItem_clicked();
}

void Widget::on_howToUse_clicked()
{
    const QUrl regUrl(QLatin1String("https://bilibili.com/video/av49413063"));
    QDesktopServices::openUrl(regUrl);
}

void Widget::on_howToActive_clicked()
{
    QMessageBox::about(this,tr("激活"),tr("请联系作者\nQQ 1953649096\n获取激活码，价格15.8元\n加好友请说明“究极绿宝石电脑版修改器激活”"));
}
