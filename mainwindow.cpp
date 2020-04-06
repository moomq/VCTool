#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QDebug>
#include<QDate>
#include<QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{ 
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_parse_clicked()
{
    QString Error_Str="";
    QString ErrorTitle="错误警告";
    QString input_data = ui->textEdit_in->toPlainText().remove(QRegExp("\\s"));
    QString headstr=input_data.at(0);
    if(input_data.isEmpty()==true) {
        return;
    }
    for(quint16 i=2;i<input_data.size();i=i+3)
    {
        input_data.insert(i," ");
    }
    QStringList Sldata=input_data.split(" ");
    if(Sldata.size()<2) return;
//    QList Sldata=input_data.toLatin1().split(" ");
    quint8 cmdtype=Sldata.at(0).toUInt(NULL,16);
    output_data.clear();
    ui->textEdit_out->clear();
    if((input_data.left(5)=="FF FF")&&(input_data.mid(6,5)!="FF FF")){
        Error_Str.append(dealVC((Sldata.at(8).toInt(NULL,16)),Sldata));
    }
    else if(headstr=="C"){
        Error_Str.append(dealVC(cmdtype,Sldata));
    }
    else if(cmdtype==0x04&&Sldata.at(20)=="5F"&&Sldata.at(21)=="1B"){
        dealcheck(Sldata);
    }
    else if(Sldata.size()>11||(input_data.left(11)=="FF FF FF FF")){
        Error_Str.append(dealDSRC(&Sldata));
    }
    else{
        Error_Str="非法帧格式";
    }

    if(Error_Str==""){
        ui->textEdit_out->setPlainText(output_data);
        ui->textEdit_out->setStyleSheet("font-size : 20px");
    }
    else{
        QMessageBox::warning(this, ErrorTitle, Error_Str);
    }
}

QString MainWindow::dealVC(quint8 cmdtype,QStringList Sldata)
{
    QString Error_Str;
    uint TextSize=Sldata.size();
    switch (cmdtype) {
    case 0xc0:
        if(TextSize==22) dealC0(Sldata);
        else Error_Str="C0 指令长度错误";
        break;

    case 0xc1:
        if(TextSize==13) dealC1(Sldata);
        else Error_Str="C1 指令长度错误";
        break;

    case 0xC2:
        if(TextSize==10||TextSize==6) dealC2(Sldata);
        else Error_Str="C2 指令长度错误";
        break;

    case 0xc4:
        if(TextSize==2) dealC4(Sldata);
        else Error_Str="C4 指令长度错误";
        break;

    case 0xc6:
        if(TextSize==164||TextSize==67||TextSize==159) dealC6(Sldata);
        else Error_Str="C6 指令长度错误";
        break;

    case 0xc7:
        if(TextSize==14||TextSize==138) dealC7(Sldata);
        else Error_Str="C7 指令长度错误";
        break;

    case 0xc8:
        if(TextSize==22||TextSize==7) dealC8(Sldata);
        else Error_Str="C8 指令长度错误";
        break;

    case 0xca:
        if(TextSize==5||TextSize==3) dealCA(Sldata);
        else Error_Str="CA 指令长度错误";
        break;

    case 0xcb:
        dealCB(Sldata);
        break;

    case 0xcf:
        if(TextSize==2) dealCF(Sldata);
        else Error_Str="CF 指令长度错误";
        break;

    case 0xfa:
        if(TextSize==4) dealFA(Sldata);
        else Error_Str="FA 指令长度错误";
        break;

    case 0xfb:
        if(TextSize==4) dealFB(Sldata);
        else Error_Str="FB 指令长度错误";
        break;

    case 0xfc:
        if(TextSize==2||TextSize==6) dealFC(Sldata);
        else Error_Str="FC 指令长度错误";
        break;

    case 0xb0:
        dealB0(Sldata);
        break;

    case 0xb1:
        dealB1(Sldata);
        break;

    case 0xb2:
        if(TextSize==47) dealB2(Sldata);
        else Error_Str="B2 帧长度错误";
        break;

    case 0xb4:
        if(TextSize==284||TextSize==193||TextSize==74||TextSize==171) dealB4(Sldata);
        else Error_Str="B4 帧长度错误";
        break;

    case 0xb5:
        if(TextSize==46||TextSize==30) dealB5(Sldata);
        else Error_Str="B5 帧长度错误";
        break;

    case 0xb7:
        dealB7(Sldata);
        break;

    case 0xb8:
        if(TextSize==28) dealB8(Sldata);
        else Error_Str="B8 帧长度错误";
        break;

    case 0xba:
        dealBA(Sldata);
        break;

    case 0xbb:
        dealBB(Sldata);
        break;

    case 0xea:
        if(TextSize==12) dealEA(Sldata);
        else Error_Str="EA 帧长度错误";
        break;

    case 0xeb:
        dealEB(Sldata);
        break;

    case 0xec:
        if(TextSize==17) dealEC(Sldata);
        else Error_Str="EC 帧长度错误";
        break;

    default:
        Error_Str="输入错误！！！";
        break;
    }
    return Error_Str;
}

void MainWindow::dealC0(QStringList Sl_data)
{
    C0_CMD_STRTYPE c0_cmdstruct;
    QString DatetimeStr;
    QString unixtimeStr;
    QString flagidStr;
    QString resercedstr;

    c0_cmdstruct.cmd_type=0xc0;
    unixtimeStr=Sl_data.at(1)+Sl_data.at(2)+Sl_data.at(3)+Sl_data.at(4);
    c0_cmdstruct.unix_time=unixtimeStr.toUInt(NULL,16);
    QDateTime datatime = QDateTime::fromTime_t(c0_cmdstruct.unix_time);
    DatetimeStr= QString(Sl_data.at(5)+Sl_data.at(6)+Sl_data.at(7)+Sl_data.at(8)+Sl_data.at(9)+Sl_data.at(10)+Sl_data.at(11));
    c0_cmdstruct.lanemode=Sl_data.at(12).toUInt(NULL,16)&0xFF;
    c0_cmdstruct.bstinterval=Sl_data.at(13).toUInt(NULL,16)&0xFF;
    c0_cmdstruct.txpower=Sl_data.at(14).toUInt(NULL,16)&0xFF;
    c0_cmdstruct.pllchannelid=Sl_data.at(15).toUInt(NULL,16)&0xFF;
    c0_cmdstruct.transmode=Sl_data.at(16).toUInt(NULL,16)&0xFF;
    flagidStr=Sl_data.at(17)+Sl_data.at(18)+Sl_data.at(19);
    c0_cmdstruct.flagid=flagidStr.toUInt(NULL,16);
    resercedstr=Sl_data.at(20)+Sl_data.at(21);
    output_data.append("CmdType: C0 (初始化指令)\r\n");
    output_data.append(QString("unix_time: %1 (").arg(unixtimeStr));
    output_data.append(datatime.toString("yyyy-MM-dd hh:mm:ss:zzz ddd")+")\r\n");
    output_data.append("Datetime:  "+Sl_data.at(5)+Sl_data.at(6)+"年"+Sl_data.at(7)+"月"+Sl_data.at(8)+"日 "+Sl_data.at(9)+":"+Sl_data.at(10)+":"+Sl_data.at(11)+"\r\n");
    if(c0_cmdstruct.lanemode==0x11)
    {
        output_data.append(QString("lanemode: 0x11 (路段 ETC 门架)\r\n"));
    }
    else if(c0_cmdstruct.lanemode==0x12)
    {
        output_data.append(QString("lanemode: 12H (省界入口 ETC门架)\r\n"));
    }
    else if(c0_cmdstruct.lanemode==0x13)
    {
        output_data.append(QString("lanemode: 13H (省界出口 ETC 门架)\r\n"));
    }
    output_data.append(QString("bstinterval: %1 (路侧单元自动发送 BST 的间隔，单位 ms)\r\n").arg(c0_cmdstruct.bstinterval));
    output_data.append(QString("txpower: %1 (路侧单元功率级数，最大值 31（1FH）)\r\n").arg(c0_cmdstruct.txpower));
    output_data.append(QString("pllchannelid: %1 (信道号， 01H 指示信道 1， 02H 指示信道 2)\r\n").arg(c0_cmdstruct.pllchannelid));
    output_data.append(QString("transmode: %1 (01H：复合交易)\r\n").arg(c0_cmdstruct.transmode));
    output_data.append(QString("flagid: %1 (ETC门架编号 0x%2)\r\n").arg(c0_cmdstruct.flagid).arg(flagidStr));
    output_data.append(QString("reserced: 0x%1 (1字节-同一收费单元的第几排门架 1字节-预留填写 00H)\r\n").arg(resercedstr));
}

void MainWindow::dealC1(QStringList Sl_data)
{
    QString tempstr;
    QString obuidStr;

    obuidStr=Sl_data.at(1)+Sl_data.at(2)+Sl_data.at(3)+Sl_data.at(4);
        for(qint8 i=5;i<13;i++) {
            tempstr=tempstr.append(Sl_data.at(i));
        }
    output_data.append("CmdType: C1 (继续交易指令)\r\n");
    output_data.append(QString("OBUID/CPC-MAC: %1 (车载单元 MAC 地址)\r\n").arg(obuidStr));
    output_data.append(QString("OBUDivFactor: %1 (车载单元一级分散因子)\r\n").arg(tempstr));
}

void MainWindow::dealC2(QStringList Sl_data)
{
    C2_CMD_STRTYPE c2_cmdstruct;
    QString obuidStr;

    obuidStr=Sl_data.at(1)+Sl_data.at(2)+Sl_data.at(3)+Sl_data.at(4);
    c2_cmdstruct.stoptype=Sl_data.at(5).toUInt(NULL,16);
    output_data.append("CmdType: C2 (停止交易指令)\r\n");
    output_data.append(QString("OBUID/CPC-MAC: %1 (车载单元 MAC 地址)\r\n").arg(obuidStr));
    if(c2_cmdstruct.stoptype==0x01)
    {
        output_data.append("stoptype: 0x01 (结束交易，重新搜索车载单元)\r\n");
    }
    else if(c2_cmdstruct.stoptype==0x02)
    {
        output_data.append("stoptype: 0x02 (重新发送当前帧)\r\n");
    }
    else if(c2_cmdstruct.stoptype==0x03){
        output_data.append("stoptype: 0x03 (冻结当前 OBUID/CPC-MAC 的车载单元)\r\n");
    }
    else{
        output_data.append(QString("stoptype: %1 (其他值，保留)\r\n").arg(c2_cmdstruct.stoptype));
    }

    if(Sl_data.size()==10)
    {
        QString unixtimeStr=Sl_data.at(6)+Sl_data.at(7)+Sl_data.at(8)+Sl_data.at(9);
        c2_cmdstruct.unix_time=unixtimeStr.toUInt(NULL,16);
        QDateTime datatime = QDateTime::fromTime_t(c2_cmdstruct.unix_time);
        output_data.append(QString("unix_time: %1 (").arg(unixtimeStr));
        output_data.append(datatime.toString("yyyy-MM-dd hh:mm:ss:zzz ddd")+")\r\n");
    }
}

void MainWindow::dealC4(QStringList Sl_data)
{
    C4_CMD_STRTYPE c4_cmdstruct;

    c4_cmdstruct.cmd_type=0xC4;
    c4_cmdstruct.ControlType=Sl_data.at(1).toUInt(NULL,16);
    output_data.append("CmdType: C4 (开关路侧单元指令)\r\n");
    if(c4_cmdstruct.ControlType==0x00)
    {
        output_data.append("ControlType: 0x00 (关闭路侧单元)\r\n");
    }
    else if(c4_cmdstruct.ControlType==0x01)
    {
        output_data.append("ControlType: 0x01 (打开路侧单元)\r\n");
    }
    else
    {
        output_data.append(QString("ControlType: %1 (其他值：保留)\r\n").arg(c4_cmdstruct.ControlType));
    }
}


void MainWindow::dealC6(QStringList Sl_data)
{
    C6_CMD_STRTYPE c6_cmdstruct;
    QString obuidstr;
    QString tempstr;
    QString PurchaseTimeStr;
    QString StationStr;
    QString EF04InfoStr;
    QString EF04Info2Str;

    int sldatasize=Sl_data.size();
    obuidstr=Sl_data.at(1)+Sl_data.at(2)+Sl_data.at(3)+Sl_data.at(4);
    for(qint8 i=5;i<13;i++) {
        tempstr=tempstr.append(Sl_data.at(i));
    }
    c6_cmdstruct.OBUTradeType=Sl_data.at(13).toUInt(NULL,16);
    output_data.append("CmdType: C6 (消费交易指令)\r\n");
    output_data.append(QString("OBUID/CPCMAC: %1 (车载单元 MAC 地址)\r\n").arg(obuidstr));
    output_data.append(QString("OBUDivFactor/CPCDivFacto: %1 (车载单元一级分散因子)\r\n").arg(tempstr));

    if(sldatasize==164||sldatasize==159||sldatasize==174){
        if(c6_cmdstruct.OBUTradeType==0x00){
            output_data.append("OBUTradeType: 0x00 (双片式 OBU更新 EF04 文件+复合消费,对于单片式 OBU、CPC 卡本字段无效)\r\n");
        }
        else if(c6_cmdstruct.OBUTradeType==0x01){
            output_data.append("OBUTradeType: 0x01 (双片式OBU，仅更新 EF04 文件)\r\n");
        }
        else if(c6_cmdstruct.OBUTradeType==0x02){
            output_data.append("OBUTradeType: 0x02 (双片式OBU，仅进行复合消费操作)\r\n");
        }
        for(qint8 i=18;i<25;i++) {
            PurchaseTimeStr+=Sl_data.at(i);
        }
        output_data.append(QString("ConsumeMoney: "+Sl_data.at(14)+Sl_data.at(15)+Sl_data.at(16)+Sl_data.at(17)+" (本路段扣款/计费金额(高字节在前，单位分))\r\n"));
        output_data.append("PurchaseTime:  "+Sl_data.at(18)+Sl_data.at(19)+"年"+Sl_data.at(20)+"月"+Sl_data.at(21)+"日 "+Sl_data.at(22)+":"+Sl_data.at(23)+":"+Sl_data.at(24)+"\r\n");
        for(qint8 i=25;i<68;i++) {
            StationStr+=Sl_data.at(i);
        }
        if(sldatasize==174){
            output_data.append("Station: "+StationStr+" (双片式OBU： 0019 文件43字节；单片式OBU：EF07文件前43字节"+"\r\n");
        }
        else{
            output_data.append("Station: "+StationStr+" (双片式OBU： 0019 文件43字节；单片式OBU：ETC门架编号3字节+4字节UNIXTime，剩余字节补齐00H；CPC卡： DF01\\EF04文件中本省计费信息10字节，剩余字节补齐00H)"+"\r\n");
        }
        for(quint8 i=68;i<159;i++) {
            EF04InfoStr+=Sl_data.at(i);
        }
        if(sldatasize==174){
            output_data.append("EF04Info: "+EF04InfoStr+" (双片式OBU： EF04文件315-405字节内容；单片式OBU：EF07文件第44-103字节，剩余字节填充00H)"+"\r\n");
        }
        else{
            output_data.append("EF04Info: "+EF04InfoStr+" (双片式OBU： EF04文件315-405字节内容；对于单片式OBU和CPC卡本字段无效，填充00H)"+"\r\n");
        }
        if(sldatasize==164){
            for(quint8 i=159;i<164;i++) {
                EF04Info2Str+=Sl_data.at(i);
            }
            output_data.append("EF04Info2: "+EF04Info2Str+" (双片式OBU：1字节偏移值N + 4字节EF04文件信息；单片式OBU和CPC卡本字段无效，填充00H)"+"\r\n");
        }
        else if(sldatasize==174){
            for(quint8 i=159;i<174;i++) {
                EF04Info2Str+=Sl_data.at(i);
            }
            output_data.append("EF04Info2: "+EF04Info2Str+" (双片式OBU：1字节偏移值N + 4字节EF04文件信息,剩余10字节补齐00H。单片式OBU：1字节偏移值N + 14字节EF07文件信息，本字段N>0时启用，N取值范围为0-15.)"+"\r\n");
        }
    }
    else if(sldatasize==67){
        output_data.append("ConsumeMoney: "+Sl_data.at(13)+Sl_data.at(14)+Sl_data.at(15)+Sl_data.at(16)+" (本路段扣款/计费金额（高字节在前，单位分))"+"\r\n");
        output_data.append("PurchaseTime:  "+Sl_data.at(17)+Sl_data.at(18)+"年"+Sl_data.at(19)+"月"+Sl_data.at(20)+"日 "+Sl_data.at(21)+":"+Sl_data.at(22)+":"+Sl_data.at(23)+"\r\n");
        for(qint8 i=24;i<67;i++) {
            StationStr+=Sl_data.at(i);
        }
        output_data.append("Station: "+StationStr+" (双片式OBU:0019 文件43字节,单片式OBU:ETC门架编号3字节+4字节UNIXTime,剩余字节补齐00H;CPC卡:DF01\\EF04文件中本省计费信息10字节+DF01\\EF02文件前4字节+DF01\\EF02文件中本门架过站信息6字节)"+"\r\n");

    }
}

void MainWindow::dealC7(QStringList Sl_data)
{
    C7_CMD_STRTYPE c7_cmdstruct;
    QString tempstr;
    QString cpc_macstr;
    QString MTCIfo1str;
    QString MTCIfo2str;

    cpc_macstr=Sl_data.at(1)+Sl_data.at(2)+Sl_data.at(3)+Sl_data.at(4);
    for(qint8 i=5;i<13;i++) {
        tempstr=tempstr.append(Sl_data.at(i));
    }
    c7_cmdstruct.CPCDivFactor=tempstr.toULong(NULL,16);
    c7_cmdstruct.UpdateType=Sl_data.at(13).toUInt(NULL,16);
    output_data.append("CmdType: C7 (更新过站信息指令)\r\n");
    output_data.append(QString("cpc_mac: %1 (CPC 卡 MAC 地址 0x%2)\r\n").arg(cpc_macstr));
    output_data.append(QString("CPCDivFactor: %1 (CPC 卡一级分散因子 0x%2)\r\n").arg(c7_cmdstruct.CPCDivFactor).arg(tempstr));
    if(Sl_data.size()==14){
        if(c7_cmdstruct.UpdateType==0x01){
            output_data.append("UpdateType: 01H (仅清空过站信息)\r\n");
        }
        else if(c7_cmdstruct.UpdateType==0x02){
            output_data.append("UpdateType: 02H (读取所有过站信息后再清空过站信息)\r\n");
        }
        else
        {
            output_data.append(QString("UpdateType: %1 (非法数值)\r\n").arg(c7_cmdstruct.UpdateType));
        }
    }
    else if(Sl_data.size()==138){
        for(quint8 i=13;i<114;i++) {
            MTCIfo1str+=Sl_data.at(i);
        }
        output_data.append("MTCIfo1: "+MTCIfo1str+" (CPC 卡过站信息文件更新内容（前 101 字节）)");
        for(quint8 i=114;i<138;i++) {
            MTCIfo2str+=Sl_data.at(i);
        }
        output_data.append("MTCIfo1: "+MTCIfo2str+" (1 字节偏移值 N +23 字节 CPC 卡计费信息文件更新内容。表示偏移(N-1)*23 字节更新 23 字节 CPC 卡计费信息)");
    }
}

void MainWindow::dealC8(QStringList Sl_data)
{
    C8_CMD_STRTYPE c8_cmdstruct;
    QString obuidstr;
    QString EF04InfoStr;
    QString EF04Info2Str;

    obuidstr=Sl_data.at(1)+Sl_data.at(2)+Sl_data.at(3)+Sl_data.at(4);
    c8_cmdstruct.Epserial=QString(Sl_data.at(5)+Sl_data.at(6)).toUInt(NULL,16);
    if(Sl_data.size()==7){

    }
    else if(Sl_data.size()==22)
    {
        c8_cmdstruct.EF04CmdType=Sl_data.at(7).toUInt(NULL,16);
        output_data.append("CmdType: C8 (重取 TAC 指令)\r\n");
        output_data.append(QString("OBUID: %1 (车载单元 MAC 地址（OBU）)\r\n").arg(obuidstr));
        output_data.append(QString("Epserial: %1 (对应的 ETC 卡 EP/ED 交易序号)\r\n").arg(c8_cmdstruct.Epserial));
        if(c8_cmdstruct.EF04CmdType==0x00){
            output_data.append(QString("EF04CmdType: 0x00 (指示本指令无 EF04Info 字段内容， 本次重取TAC不操作EF04文件)\r\n"));
        }
        else if(c8_cmdstruct.EF04CmdType==0x01){
            output_data.append(QString("EF04CmdType: 0x01 (指示本指令有 EF04Info 字段内容， 本次重取TAC操作EF04文件，后续字节有效)\r\n"));
            for(quint8 i=8;i<99;i++) {
                EF04InfoStr=EF04InfoStr.append(Sl_data.at(i));
            }
            output_data.append(QString("EF04InfoStr: %1\r\n").arg(EF04InfoStr));
            for(quint8 i=99;i<104;i++) {
                EF04Info2Str=EF04Info2Str.append(Sl_data.at(i));
            }
            output_data.append(QString("EF04Info2Str: %1\r\n").arg(EF04Info2Str));
        }
        else{
            output_data.append(QString("EF04CmdType: %1 (非法数值)\r\n").arg(c8_cmdstruct.EF04CmdType));
        }
    }
}

void MainWindow::dealCA(QStringList Sl_data)
{

    QString PSAMChannelstr;
    if(Sl_data.size()==5){
        PSAMChannelstr=Sl_data.at(1)+Sl_data.at(2)+Sl_data.at(3)+Sl_data.at(4);
    }
    else if(Sl_data.size()==3){
        PSAMChannelstr=Sl_data.at(1)+Sl_data.at(2);
    }
    output_data.append("CmdType: CA (PSAM 初始化指令)\r\n");
    output_data.append(QString("PSAMChannel: 0x%1 (按位表示需要授权的 PSAM 通道号（支持通道号1~通道号16）)\r\n").arg(PSAMChannelstr));

}

void MainWindow::dealCB(QStringList Sl_data)
{
    CB_CMD_STRTYPE cb_cmdstruct;
    QString AuthInfoStr;

    cb_cmdstruct.cmd_type=0xCB;
    cb_cmdstruct.PSAMCount=Sl_data.at(1).toUInt(NULL,16);
    output_data.append("CmdType: CB (PSAM 授权指令)\r\n");
    output_data.append(QString("PSAMCount: %1 (路侧单元需要授权的 PSAM 数量，记为 n)\r\n").arg(cb_cmdstruct.PSAMCount));
    if(cb_cmdstruct.PSAMCount>0){
        for(quint8 i=2;i<(15*cb_cmdstruct.PSAMCount+2);i++) {
            if((i-2)%15==0) AuthInfoStr.append(" ");
            AuthInfoStr=AuthInfoStr.append(Sl_data.at(i));
        }
        output_data.append(QString("AuthInfo:%1 (1 字节PSAM 通道号；1 字节认证状态，00H 正常，其他故障；13字节授权认证数据)\r\n").arg(AuthInfoStr));
    }
}
void MainWindow::dealCF(QStringList Sl_data)
{
    CF_CMD_STRTYPE cf_cmdstruct;

    cf_cmdstruct.cmd_type=0xCF;
    output_data.append("CmdType: CF (心跳应答指令)\r\n");
    if(Sl_data.length()>1){
        cf_cmdstruct.Status=Sl_data.at(1).toUInt(NULL,16);
        output_data.append(QString("Status: %1 (00：接收成功，其他保留)\r\n").arg(cf_cmdstruct.Status));
    }
}

void MainWindow::dealFA(QStringList Sl_data)
{
    FA_CMD_STRTYPE fa_cmdstruct;

    fa_cmdstruct.cmd_type=0xCF;
    fa_cmdstruct.RSUID=Sl_data.at(1).toUInt(NULL,16);
    fa_cmdstruct.TxStatus=Sl_data.at(2).toUInt(NULL,16);
    fa_cmdstruct.TxType=Sl_data.at(3).toUInt(NULL,16);
    output_data.append("CmdType: FA (测试信号发送指令)\r\n");
    if(fa_cmdstruct.RSUID==0x00){
        output_data.append(QString("RSUID: 0x00 (所有 RSU 天线头)\r\n"));
    }
    else{
        output_data.append(QString("RSUID: %1 (左边起第%1个RSU天线头)\r\n").arg(fa_cmdstruct.RSUID));
    }
    output_data.append(QString("TxStatus: %1 (00关闭发射测试模式;01开始发射测试模式)\r\n").arg(fa_cmdstruct.TxStatus));
    switch (fa_cmdstruct.TxType) {
    case 0x00:
        output_data.append("TxType: 0x00 (载波信号) \r\n");
        break;
    case 0x01:
        output_data.append("TxType: 0x01 (PN9 信号) \r\n");
        break;
    case 0x02:
        output_data.append("TxType: 0x02 (全 0 信号) \r\n");
        break;
    case 0x03:
        output_data.append("TxType: 0x03 (全 1 信号) \r\n");
        break;
    default:
        output_data.append(QString("TxType: %1 (非法数值) \r\n").arg(fa_cmdstruct.TxType));
        break;
    }
}

void MainWindow::dealFB(QStringList Sl_data)
{
    FB_CMD_STRTYPE fb_cmdstruct;

    fb_cmdstruct.cmd_type=0xCF;
    fb_cmdstruct.RSUID=Sl_data.at(1).toUInt(NULL,16);
    fb_cmdstruct.RxStatus=Sl_data.at(2).toUInt(NULL,16);
    fb_cmdstruct.IfRevDataDiff=Sl_data.at(3).toUInt(NULL,16);
    output_data.append("CmdType: FB (测试信号接收指令)\r\n");
    if(fb_cmdstruct.RSUID==0x00){
        output_data.append(QString("RSUID: 0x00 (所有 RSU 天线头（暂不要求）)\r\n"));
    }
    else{
        output_data.append(QString("RSUID: %1 (左边起第%1个RSU天线头)\r\n").arg(fb_cmdstruct.RSUID));
    }
    output_data.append(QString("RxStatus: %1 (00H停止接收测试模式;01H启动接收测试模式，其他保留)\r\n").arg(fb_cmdstruct.RxStatus));
    if(fb_cmdstruct.IfRevDataDiff==0x00){
        output_data.append("IfRevDataDiff: 0x00 (接收到的数据帧与第一个数据不进行比较)\r\n");
    }
    else if(fb_cmdstruct.IfRevDataDiff==0x01){
        output_data.append("IfRevDataDiff: 0x01 (接收到的数据帧与第一个数据进行比较)\r\n");
    }
    else{
        output_data.append(QString("IfRevDataDiff: %1 (保留值)\r\n").arg(fb_cmdstruct.IfRevDataDiff));
    }
}
void MainWindow::dealFC(QStringList Sl_data)
{
    FC_CMD_STRTYPE fc_cmdstruct;

    fc_cmdstruct.cmd_type=0xCF;
    fc_cmdstruct.BeaconIDStatus=Sl_data.at(1).toUInt(NULL,16);
    output_data.append("CmdType: FC (BeaconID 设置指令)\r\n");
    switch (fc_cmdstruct.BeaconIDStatus) {
    case 0x00:
        output_data.append("BeaconIDStatus: 0x00 (设置 BeaconID 为固定值: )"+Sl_data.at(2)+Sl_data.at(3)+Sl_data.at(4)+Sl_data.at(5));
        break;
    case 0x01:
         output_data.append("BeaconIDStatus: 0x01 (设置 BeaconID 为变化状态)");
        break;
    case 0x02:
        output_data.append("BeaconIDStatus: 0x02 (查询 BeaconID 状态)");
        break;
    default:
        output_data.append(QString("BeaconIDStatus: 1% (保留值)").arg(fc_cmdstruct.BeaconIDStatus));
        break;
    }
}

void MainWindow::dealB0(QStringList Sl_data)
{
    B0_CMD_STRTYPE b0_cmdstruct;
    QString RSUStatusStr;
    QString PSAMInfo1str;
    QString RSUIDstr;
    QString RSUVersionstr;
    QString FlagIDstr;
    QString Reserved;
    uint offset=0;

    b0_cmdstruct.FrameType=0xCF;
    b0_cmdstruct.RSUStatus=Sl_data.at(9).toUInt(NULL,16);
    RSUStatusStr=dealB0StatusCode(b0_cmdstruct.RSUStatus);
    b0_cmdstruct.PSAMNum1=Sl_data.at(10).toUInt(NULL,16);
    output_data.append("FrameType: B0 (设备状态信息帧)\r\n");
    output_data.append(QString("RSUStatus: %1H (%2)\r\n").arg(Sl_data.at(9)).arg(RSUStatusStr));
    output_data.append(QString("PSAMNum1: %1 (路侧单元中 PSAM 数量)\r\n").arg(b0_cmdstruct.PSAMNum1));
    offset=9*b0_cmdstruct.PSAMNum1+3+8;
    if(b0_cmdstruct.PSAMNum1>0){
        for(uint i=11;i<offset;i++){
            PSAMInfo1str=PSAMInfo1str.append(Sl_data.at(i));
        }
        for(qint8 j=0;j<b0_cmdstruct.PSAMNum1;j++){
            output_data.append(QString("PSAM%1Info: %2 ").arg(j+1).arg(PSAMInfo1str.mid(j*18,18)));
        }
        output_data.append("(1字节PSAM通道号,1字节PSAM版本号,1字节PSAM授权状态,00H已授(含无需授权),01H未授权,6字节PSAM终端机编号)\r\n");
    }
    b0_cmdstruct.RSUAlgId=Sl_data.at(offset++).toUInt(NULL,16);
    QString RSUManuIDstr=Sl_data.at(offset++);
    RSUIDstr=Sl_data.at(offset)+Sl_data.at(offset+1)+Sl_data.at(offset+2);
    offset+=3;
    RSUVersionstr=Sl_data.at(offset)+Sl_data.at(offset+1);
    offset+=2;
    b0_cmdstruct.workstatus=QString(Sl_data.at(offset)).toInt(NULL,16);
    offset++;
    FlagIDstr=Sl_data.at(offset)+Sl_data.at(offset+1)+Sl_data.at(offset+2);
    offset+=3;
    Reserved=Sl_data.at(offset)+Sl_data.at(offset+1)+Sl_data.at(offset+2)+Sl_data.at(offset+3);
    output_data.append(QString("RSUAlgId: %1 (算法标识，默认填写 00H)\r\n").arg(b0_cmdstruct.RSUAlgId));
    output_data.append(QString("RSUManuID: %1 (路侧单元厂商代码)\r\n").arg(RSUManuIDstr));
    output_data.append(QString("RSUIDstr: %1 (路侧单元编号，须与 BeaconID 保持一致)\r\n").arg(RSUIDstr));
    output_data.append(QString("RSUVersion: %1 (路侧单元软件版本号，采用本标准版本不低于 2000H)\r\n").arg(RSUVersionstr));
    output_data.append(QString("workstatus: %1 (工作模式返回状态，默认填写 00H)\r\n").arg(b0_cmdstruct.workstatus));
    output_data.append(QString("FlagIDstr: %1 (ETC 门架编号（由 C0 帧中获取，获取失败填充 00H）)\r\n").arg(FlagIDstr));
    output_data.append(QString("Reserved: %1 (前 2 字节指示接口版本信息，后 2 字节保留填充00H。即依据接口 V3.4 开发时，此处填充 03040000H)\r\n").arg(Reserved));
}

void MainWindow::dealB1(QStringList Sl_data)
{
    B1_CMD_STRTYPE b1_cmdstruct;
    QString PSAMStatus1str;
    QString PSAMStatus2str;
    QString AntennaStatusstr;
    QString RSUControlStatus1str;
    QString RSUControlStatus2str;
    int temp;

    int offset=9;
    b1_cmdstruct.RSUControlStatus1=Sl_data.at(offset++).toUInt(NULL,16);
    b1_cmdstruct.RSUControlStatus2=Sl_data.at(offset++).toUInt(NULL,16);
    b1_cmdstruct.RSUControlStatus3=Sl_data.at(offset++).toUInt(NULL,16);
    RSUControlStatus1str=dealRSUControlStatus(b1_cmdstruct.RSUControlStatus1);
    RSUControlStatus2str=dealRSUControlStatus(b1_cmdstruct.RSUControlStatus1);
    b1_cmdstruct.PSAMNum1=Sl_data.at(offset++).toUInt(NULL,16);
    temp=offset;
    for(;offset<3*b1_cmdstruct.PSAMNum1+temp;offset++){
        if((offset-temp)%3==0) PSAMStatus1str.append(" ");
        PSAMStatus1str= PSAMStatus1str.append(Sl_data.at(offset));
    }
    b1_cmdstruct.PSAMNum2=Sl_data.at(offset++).toUInt(NULL,16);
    temp=offset;
    for(;offset<3*b1_cmdstruct.PSAMNum2+temp;offset++){
        if((offset-temp)%3==0) PSAMStatus2str.append(" ");
        PSAMStatus2str= PSAMStatus2str.append(Sl_data.at(offset));
    }
    b1_cmdstruct.RSUAntennaNum=Sl_data.at(offset++).toUInt(NULL,16);
    b1_cmdstruct.RSUAntennaNum2=Sl_data.at(offset++).toUInt(NULL,16);
    temp=offset;
    for(;offset<4*b1_cmdstruct.RSUAntennaNum+temp;offset++){
        if((offset-temp)%4==0) AntennaStatusstr.append(" ");
        AntennaStatusstr= AntennaStatusstr.append(Sl_data.at(offset));
    }
    output_data.append("CmdType: B1 (心跳信息帧)\r\n");
    output_data.append(QString("RSUControlStatus1: %1H (路侧单元控制器1状态: %2)\r\n").arg(Sl_data.at(9)).arg(RSUControlStatus1str));
    output_data.append(QString("RSUControlStatus2: %1H (路侧单元控制器2状态: %2)\r\n").arg(Sl_data.at(10)).arg(RSUControlStatus2str));
    output_data.append(QString("RSUControlStatus3: %1H (路侧单元主备控制器之间的通信连接状态: ").arg(Sl_data.at(11)));
    if(b1_cmdstruct.RSUControlStatus3==0x00) output_data.append("通讯正常)\r\n");
    else{
        output_data.append("通讯异常\r\n");
    }
    output_data.append(QString("PSAMNum1: %1 (路侧单元控制器1 PSAM 数量)\r\n").arg(b1_cmdstruct.PSAMNum1));
    output_data.append(QString("PSAMStatus1: %1 (1字节PSAM通道号;1字节PSAM运行状态,00H正常,01H异常,1字节PSAM卡授权状态,00H已授权,01H未授权(含授权失败))\r\n").arg(PSAMStatus1str));
    output_data.append(QString("PSAMNum2: %1 (路侧单元控制器1 PSAM 数量)\r\n").arg(b1_cmdstruct.PSAMNum2));
    output_data.append(QString("PSAMStatus2: %1 (1字节PSAM通道号;1字节PSAM运行状态,00H正常,01H异常,1字节PSAM卡授权状态,00H已授权,01H未授权(含授权失败))\r\n").arg(PSAMStatus2str));
    output_data.append(QString("RSUAntennaNum: %1 (路侧单元配置的天线数量)\r\n").arg(b1_cmdstruct.RSUAntennaNum));
    output_data.append(QString("RSUAntennaNum2: %1 (主用路侧单元控制器所连接天线中正常工作的数量)\r\n").arg(b1_cmdstruct.RSUAntennaNum2));
    output_data.append(QString("AntennaStatusstr: %1 (1字节天线ID编号;1字节天线运行状态 00H正常,01H通讯状态异常(控制器与天线之间),02H天线射频模块状态异常,03H其他异常;1字节天线信道,00H信道1,01H信道2;1字节功率等级,取值00H～1FH)\r\n").arg(AntennaStatusstr));
}

void MainWindow::dealB2(QStringList Sl_data)
{
    B2_CMD_STRTYPE b2_cmdstruct;

    int offset;
    int temp;
    QString obuidstr;
    QString IssuerIdentifierstr;
    QString SerialNumberstr;
    QString DateofIssuestr;
    QString DateofExpirestr;
    QString StatusStr;
    b2_cmdstruct.FrameType=0xB2;
    obuidstr=Sl_data.at(9)+Sl_data.at(10)+Sl_data.at(11)+Sl_data.at(12);
    b2_cmdstruct.OBUID_CPCMAC=obuidstr.toUInt(NULL,16);
    b2_cmdstruct.ErrorCode=Sl_data.at(13).toUInt(NULL,16);
    b2_cmdstruct.AntennaID=Sl_data.at(14).toUInt(NULL,16);
    b2_cmdstruct.DeviceType=Sl_data.at(15).toUInt(NULL,16);
    for(offset=16;offset<24;offset++){
        IssuerIdentifierstr=IssuerIdentifierstr.append(Sl_data.at(offset));
    }
    QString ContractTypestr=Sl_data.at(offset++);
    QString ContractVersionstr=Sl_data.at(offset++);
    temp=offset;
    for(;offset<temp+8;offset++){
        SerialNumberstr=SerialNumberstr.append(Sl_data.at(offset));
    }
    DateofIssuestr=Sl_data.at(offset)+Sl_data.at(offset+1)+Sl_data.at(offset+2)+Sl_data.at(offset+3);
    offset+=4;
    DateofExpirestr=Sl_data.at(offset)+Sl_data.at(offset+1)+Sl_data.at(offset+2)+Sl_data.at(offset+3);
    offset+=4;
    b2_cmdstruct.EquitmentCV=Sl_data.at(offset++).toUInt(NULL,16);
    quint8 Status=Sl_data.at(offset).toUInt(NULL,16);
    quint8 teardownstatus=Sl_data.at(offset+1).toUInt(NULL,16);
    StatusStr=dealB2StatusCode(b2_cmdstruct.DeviceType,Status);
    output_data.append("CmdType: B2 (车载单元信息帧)\r\n");
    output_data.append(QString("OBUID/CPC-MAC: %1 (OBUID/CPC-MAC地址)\r\n").arg(obuidstr));
    output_data.append(QString("ErrorCode: %1 (执行状态代码,取值00H)\r\n").arg(b2_cmdstruct.ErrorCode));
    output_data.append(QString("AntennaID: %1 (1 字节本次交易的天线ID编号)\r\n").arg(b2_cmdstruct.AntennaID));
    if(b2_cmdstruct.DeviceType==0x01) {
        output_data.append("DeviceType: 01H (车载单元类型为双片式OBU)\r\n");
    }
    else if(b2_cmdstruct.DeviceType==0x02){
        output_data.append("DeviceType: 02H (车载单元类型为单片式OBU)\r\n");
    }
    else if(b2_cmdstruct.DeviceType==0x03)
    {
        output_data.append("DeviceType: 03H (车载单元类型为CPC卡)\r\n");
    }
    else{
        output_data.append(QString("DeviceType: %1 (车载单元类型: 01H为双片式OBU,02H为单片式OBU,03H为CPC卡,其他值保留)\r\n").arg(b2_cmdstruct.DeviceType));
    }
    output_data.append(QString("IssuerIdentifier: %1H (发行商代码)\r\n").arg(IssuerIdentifierstr));
    output_data.append(QString("ContractType: %1H (OBU 协约类型,CPC 卡时填充00H)\r\n").arg(ContractTypestr));
    output_data.append(QString("ContractVersion: %1H (OBU/CPC 合同版本)\r\n").arg(ContractVersionstr));
    output_data.append(QString("SerialNumber: %1 (OBU 合同序列号/ CPC卡ID号)\r\n").arg(SerialNumberstr));
    output_data.append(QString("DateofIssue: %1 (合同签署日期)\r\n").arg(DateofIssuestr));
    output_data.append(QString("DateofExpire: %1 (合同过期日期)\r\n").arg(DateofExpirestr));
    output_data.append(QString("EquitmentCV: %1H (OBU 设备类型及版本/CPC卡时填充00H)\r\n").arg(Sl_data.at(offset-1)));
    output_data.append(QString("OBU/CPC Status: %1 ").arg(StatusStr));
    output_data.append(QString("ESAM 系统信息第 27字节“拆卸状态”: %1 \r\n").arg(teardownstatus));
}


void MainWindow::dealB4(QStringList Sl_data)
{
    int offset;
    int temp;
    QString obuidstr;
    QString ErrorCodeStr;
    QString VehicleInfostr;

    QString CardRestMoneystr;
    QString EF04Infostr;
    QString IssuerInfostr;
    QString LastStationstr;
    QString cpcLastStationstr;
    QString EF04CostMoneyInfostr;
    quint8 ErrorCode=Sl_data.at(13).toUInt(NULL,16);
    int datasize=Sl_data.size();
    ErrorCodeStr=dealB4ErrorCode(ErrorCode);
    obuidstr=Sl_data.at(9)+Sl_data.at(10)+Sl_data.at(11)+Sl_data.at(12);
    output_data.append("CmdType: B4 (用户信息帧)\r\n");
    output_data.append(QString("OBUID/CPC-MAC: %1 (OBUID/CPC-MAC地址)\r\n").arg(obuidstr));
    output_data.append(QString("ErrorCode: %1H (%2)\r\n").arg(Sl_data.at(13)).arg(ErrorCodeStr));
    output_data.append(QString("TransType: %1H (交易类型10H：复合交易)\r\n").arg(Sl_data.at(14)));
    if(datasize==284||datasize==193){
        temp=15;
        for(offset=temp;offset<temp+79;offset++){
            VehicleInfostr=VehicleInfostr.append(Sl_data.at(offset));
        }
        CardRestMoneystr=Sl_data.at(offset)+Sl_data.at(offset+1)+Sl_data.at(offset+2)+Sl_data.at(offset+3);
        offset+=4;
        temp=offset;
        if(datasize==284){
            for(;offset<temp+91;offset++){
                EF04Infostr=EF04Infostr.append(Sl_data.at(offset));   //EF04文件
            }
            temp=offset;
        }
        for(;offset<temp+50;offset++){
            IssuerInfostr=IssuerInfostr.append(Sl_data.at(offset));
        }
        temp=offset;
        for(;offset<temp+43;offset++){
            LastStationstr=LastStationstr.append(Sl_data.at(offset));
        }
        output_data.append(QString("VehicleInfo: %1 (车辆信息文件 79 字节)\r\n").arg(VehicleInfostr));
        output_data.append(QString("CardRestMoney: %1 (电子钱包文件,单片式 OBU 填充 00H,无数据返回时填充00H)\r\n").arg(CardRestMoneystr));
        if(datasize==284){
            output_data.append(QString("EF04Info: %1 (双片式OBU EF04文件315-405，单片式 OBU：EF07文件前43 字节+后 7 字节补00H;无数据返回时填充 00H)\r\n").arg(EF04Infostr));
        }
        output_data.append(QString("IssuerInfo: %1 (卡片发行信息（0015 文件） 43 字节+后 7 字节补00H；单片式 OBU：EF07文件前 43字节+后7字节补00H，无数据返回时填充 00H)\r\n").arg(IssuerInfostr));
        output_data.append(QString("LastStation: %1 (出入口信息 双片式OBU为0019文件,单片式OBU为DF01\\EF02文件,无数据返回时填充00H)\r\n").arg(LastStationstr));
    }
    else if(datasize==74||datasize==171){
        temp=15;
        for(offset=temp;offset<temp+43;offset++){
            cpcLastStationstr=cpcLastStationstr.append(Sl_data.at(offset));
        }
        QString EF02StationInfostr=Sl_data.at(offset)+Sl_data.at(offset+1)+Sl_data.at(offset+2)+Sl_data.at(offset+3);
        offset+=4;
        temp=offset;
        for(;offset<temp+10;offset++){
            EF04CostMoneyInfostr=EF04CostMoneyInfostr.append(Sl_data.at(offset));
        }
        output_data.append(QString("LastStation: %1 (CPC 卡出入口信息)\r\n").arg(cpcLastStationstr));
        if(datasize==74){
            output_data.append(QString("EF02过站信息: %1 (CPC卡DF01\EF02文件前4字节)\r\n").arg(EF02StationInfostr));
            output_data.append(QString("EF04计费信息: %1 (CPC卡DF01\EF04文件中本省计费信息10字节)\r\n").arg(EF04CostMoneyInfostr));
        }
        else{
            QString MTCInfoStr;
            for(int i=58;i<159;i++){
                MTCInfoStr=MTCInfoStr.append(Sl_data.at(i));
            }
            output_data.append(QString("MTCInfo: %1 (CPC卡DF01\EF02文件前101字节。无数据返回时，填充00H)\r\n").arg(MTCInfoStr));
        }
    }
    else{
        qDebug()<<"数据长度错误";
    }

}

void MainWindow::dealB5(QStringList Sl_data)
{

    QString obuidstr;
    QString PSAMNostr;
    QString TransTimestr;
    obuidstr=Sl_data.at(9)+Sl_data.at(10)+Sl_data.at(11)+Sl_data.at(12);
    quint8 ErrorCode=Sl_data.at(13).toUInt(NULL,16);
    QString ErrorCodeStr=dealB5ErrorCode(ErrorCode);
    PSAMNostr=Sl_data.at(14)+Sl_data.at(15)+Sl_data.at(16)+Sl_data.at(17)+Sl_data.at(18)+Sl_data.at(19);
    TransTimestr=Sl_data.at(20)+Sl_data.at(21)+"年"+Sl_data.at(22)+"月"+Sl_data.at(23)+"日 "+Sl_data.at(24)+":"+Sl_data.at(25)+":"+Sl_data.at(26);
    output_data.append("CmdType: B5 (交易信息帧)\r\n");
    output_data.append(QString("OBUID/CPC-MAC: %1 (OBUID/CPC-MAC地址)\r\n").arg(obuidstr));
    output_data.append(QString("ErrorCode: %1H (%2)\r\n").arg(Sl_data.at(13)).arg(ErrorCodeStr));
    output_data.append(QString("PSAMNo: %1 (PSAM 卡终端机编号)\r\n").arg(PSAMNostr));
    output_data.append(QString("TransTime: %1 \r\n").arg(TransTimestr));
    output_data.append(QString("TransType: %1H (双片式OBU:取值为09H;单片式OBU:取值80H、81H,CPC取值为09H)\r\n").arg(Sl_data.at(27)));
    if(Sl_data.size()==46){
        output_data.append(QString("AlgFlag: %1H (算法标识,3DES算法00H,SM4算法04H)\r\n").arg(Sl_data.at(28)));
        output_data.append(QString("KeyVersion: %1H (双片式OBU:消费密钥版本号 单片式OBU:填写00H)\r\n").arg(Sl_data.at(29)));
        output_data.append("TAC: "+Sl_data.at(30)+Sl_data.at(31)+Sl_data.at(32)+Sl_data.at(33)+" (交易认证码,单片式 OBU：填写00H)\r\n");
        output_data.append("ICCPayserial: "+Sl_data.at(34)+Sl_data.at(35)+" (CPU 用户卡脱机交易序号单片式OBU填充00H)\r\n");
        output_data.append("PSAMTransSerial: "+Sl_data.at(36)+Sl_data.at(37)+Sl_data.at(38)+Sl_data.at(39)+" (PSAM 卡终端交易序号,单片式OBU填充 00H，双片式OBU不进行复合消费操作时填写00H)\r\n");
        QString CardBalancestr=Sl_data.at(40)+Sl_data.at(41)+Sl_data.at(42)+Sl_data.at(43);
        output_data.append("CardBalance: "+CardBalancestr+" (交易后余额,单片式OBU填充 00H)\r\n");
    }
    else if(Sl_data.size()==30){
//CPC 卡该数据域不存在
    }
    else{
        qDebug()<<"数据长度错误";
    }

}

void MainWindow::dealB7(QStringList Sl_data)
{

    QString cpcmacstr;
    QString FlagIDstr;
    quint8 ErrorCode=Sl_data.at(13).toInt(NULL,16);
    QString ErrorCodeStr=dealB7ErrorCode(ErrorCode);
    quint8 WritedFlags=Sl_data.at(14).toInt(NULL,16);
    if(WritedFlags>84) WritedFlags=84;
    cpcmacstr=Sl_data.at(9)+Sl_data.at(10)+Sl_data.at(11)+Sl_data.at(12);
    for(int i=18;i<6*WritedFlags+18;i++){
        FlagIDstr=FlagIDstr.append(Sl_data.at(i));
    }
    output_data.append("FrameType: B7 (过站信息帧)\r\n");
    output_data.append(QString("CPC-MAC: %1 (CPC-MAC地址)\r\n").arg(cpcmacstr));
    output_data.append(QString("ErrorCode: %1H (%2)\r\n").arg(Sl_data.at(13)).arg(ErrorCodeStr));
    output_data.append(QString("WritedFlags: %1H (CPC 卡过站信息文件第一字节)\r\n").arg(Sl_data.at(14)));
    output_data.append("LastFalgID:"+Sl_data.at(15)+Sl_data.at(16)+Sl_data.at(17)+" (CPC 卡已写入最新ETC门架信息)\r\n");
    output_data.append(QString("FlagID: %1 (CPC卡内已有所有过站信息,依据WritedFlags字段数量(若n＞84则取n取值84)*6字节(3字节ETC门架信息+3字节本路段计费金额))\r\n").arg(FlagIDstr));
}

void MainWindow::dealB8(QStringList Sl_data)
{

    quint8 ErrorCode=Sl_data.at(13).toInt(NULL,16);
    QString ErrorCodeStr=dealB7ErrorCode(ErrorCode);
    QString CardRestMoneystr=Sl_data.at(22)+Sl_data.at(23)+Sl_data.at(24)+Sl_data.at(25);
    output_data.append("FrameType: B8 (过站信息帧)\r\n");
    output_data.append("OBUID/CPC-MAC: "+Sl_data.at(9)+Sl_data.at(10)+Sl_data.at(11)+Sl_data.at(12)+" (CPC-MAC地址)\r\n");
    output_data.append(QString("ErrorCode: %1H (%2)\r\n").arg(Sl_data.at(13)).arg(ErrorCodeStr));
    output_data.append("TAC:"+Sl_data.at(14)+Sl_data.at(15)+Sl_data.at(16)+Sl_data.at(17)+" (交易 TAC 码)\r\n");
    output_data.append("MAC:"+Sl_data.at(18)+Sl_data.at(19)+Sl_data.at(20)+Sl_data.at(21)+" (交易 MAC 码)\r\n");
    output_data.append("CardRestMoney:"+CardRestMoneystr+" (交易后余额: "+CardRestMoneystr.toUInt(NULL,16)+",单片式OBU填充 00H)\r\n");

}
void MainWindow::dealBA(QStringList Sl_data)
{

    QString Datetimestr;
    int PSAMCount;
    QString PSAMInfostr;
    quint8 ErrorCode=Sl_data.at(9).toInt(NULL,16);
    QString ErrorCodeStr=dealB7ErrorCode(ErrorCode);
    output_data.append("FrameType: BA (过站信息帧)\r\n");
    output_data.append(QString("ErrorCode: %1H (%2)\r\n").arg(Sl_data.at(9)).arg(ErrorCodeStr));
    Datetimestr=Sl_data.at(10)+Sl_data.at(11)+"年"+Sl_data.at(12)+"月"+Sl_data.at(13)+"日 "+Sl_data.at(14)+":"+Sl_data.at(15)+":"+Sl_data.at(16);
    output_data.append(QString("Datetime: %1\r\n").arg(Datetimestr));
    PSAMCount=Sl_data.at(17).toInt(NULL,16);
    output_data.append(QString("PSAMCount: %1 (授权的PSAM数量)\r\n").arg(PSAMCount));
    for(int i=18;i<35*PSAMCount+18;i++){
        PSAMInfostr=PSAMInfostr.append(Sl_data.at(i));
    }
    output_data.append(QString("PSAMInfo: %1 (PSAM初始化信息35字节) ").arg(PSAMInfostr));
    output_data.append(QString("PSAM通道号: %1 ,PSAM终端号: %2 ,PSAM序列号: %3 ,PSAM版本号: %4 ,").arg(PSAMInfostr.left(2)).arg(PSAMInfostr.mid(2,12)).arg(PSAMInfostr.mid(14,20)).arg(PSAMInfostr.mid(34,2)));
    output_data.append(QString("PSAM密钥卡类型: %1 ,PSAM应用区域标识: %2 ,PSAM随机数 %3\r\n").arg(PSAMInfostr.mid(36,2)).arg(PSAMInfostr.mid(38,16)).arg(PSAMInfostr.mid(54,16)));
}

void MainWindow::dealBB(QStringList Sl_data)
{
    int PSAMCount;
    int ErrorCode;
    QString ErrorCodestr;
    QString AuthResultstr;

    PSAMCount=Sl_data.at(10).toInt(NULL,16);
    ErrorCode=Sl_data.at(9).toInt(NULL,16);
    ErrorCodestr=dealBBErrorCode(ErrorCode);
    output_data.append("FrameType: BB (过站信息帧)\r\n");
    output_data.append(QString("ErrorCode: %1H (%2)\r\n").arg(Sl_data.at(9)).arg(ErrorCodestr));
    output_data.append(QString("PSAMCount: %1 (授权的PSAM数量)\r\n").arg(PSAMCount));
    for(int i=11;i<4*PSAMCount+11;i++){
        if((i-11)%4==0) AuthResultstr.append(" ");
        AuthResultstr=AuthResultstr.append(Sl_data.at(i));
    }
    output_data.append(QString("AuthResult:%1 (PSAM卡授权结果信息4字节: 1字节PSAM通道号; 1字节执行状态码,00H执行成功,其他失败; 2字节授权认证TimeCos状态码,9000H 执行成功，其他失败)\r\n").arg(AuthResultstr));
}

void MainWindow::dealEA(QStringList Sl_data)
{
    int Status=Sl_data.at(9).toInt(NULL,16);

    output_data.append("CmdType: EA (测试信号发送信息帧)\r\n");
    if(Status==0){
        output_data.append("Status: 00H (正确执行)\r\n");
    }
    else if(Status==1){
        output_data.append("Status: 01H (执行失败)\r\n");
    }
    else{
        output_data.append(QString("Status: %1H (保留值)\r\n").arg(Sl_data.at(9)));
    }
}

void MainWindow::dealEB(QStringList Sl_data)
{
    QString Primitivestr;
    int Status=Sl_data.at(9).toInt(NULL,16);
    int datasize=Sl_data.size();
    quint16 RevNum=QString(Sl_data.at(10)+Sl_data.at(11)).toUInt(NULL,16);
    quint16 DiffRevNum=QString(Sl_data.at(datasize-6)+Sl_data.at(datasize-5)).toUInt(NULL,16);
    quint16 RevNum_CRCErr=QString(Sl_data.at(datasize-4)+Sl_data.at(datasize-3)).toUInt(NULL,16);


    output_data.append("CmdType: EB (测试信号接收信息帧)\r\n");
    if(Status==0){
        output_data.append("Status: 00H (正确执行)\r\n");
    }
    else if(Status==1){
        output_data.append("Status: 01H (执行失败)\r\n");
    }
    else{
        output_data.append(QString("Status: %1H (保留值)\r\n").arg(Sl_data.at(9)));
    }
    output_data.append(QString("RevNum: %1 (正确接收到的数据帧个数)\r\n").arg(RevNum));
    if(RevNum==0){
        Primitivestr="";
    }
    else{
        for(int i=12;i<datasize-18+12;i++){
            Primitivestr=Primitivestr.append(Sl_data.at(i));
        }
    }
    output_data.append(QString("Primitive: %1 (返回当前测试接收到的第一个上行原语;RevNum为0时此数据域长度为0)\r\n").arg(Primitivestr));
    output_data.append(QString("DiffRevNum: %1 (FB指令中IfRevDataDiff=01H时,与接收到的第一个上行原语不同的数据帧个数)\r\n").arg(DiffRevNum));
    output_data.append(QString("RevNum_CRCErr: %1 (接收到完整但CRC校验错误帧的数据帧个数,收到启动接收测试指令(RxStatus=01H 的 FB 指令)时清零)\r\n").arg(RevNum_CRCErr));
}

void MainWindow::dealEC(QStringList Sl_data)
{
    int Status=Sl_data.at(9).toInt(NULL,16);
    int BeaconIDStatus=Sl_data.at(10).toInt(NULL,16);

    output_data.append("CmdType: EC (BeaconID 设置信息帧)\r\n");
    if(Status==0){
        output_data.append("Status: 00H (正确执行)\r\n");
    }
    else if(Status==1){
        output_data.append("Status: 01H (执行失败)\r\n");
    }
    else{
        output_data.append(QString("Status: %1H (保留值)\r\n").arg(Sl_data.at(9)));
    }
    if(BeaconIDStatus==0){
        output_data.append("Status: 00H (BeaconID 为固定状态)\r\n");
    }
    else if(BeaconIDStatus==1){
        output_data.append("Status: 01H (BeaconID 为变化状态)\r\n");
    }
    else{
        output_data.append(QString("Status: %1H (保留值)\r\n").arg(Sl_data.at(9)));
    }
    output_data.append("BeaconID: "+Sl_data.at(11)+Sl_data.at(12)+Sl_data.at(13)+Sl_data.at(14)+" (当前 BeaconID 值)\r\n");
}

void MainWindow::dealcheck(QStringList Sl_data)
{
    int offset = 0;
    int temp = 0;
    quint8 psam1status;
    quint8 psam2status;

    QByteArray VersionArr;
    QByteArray ant_versionArr;
    QByteArray datestr;
    output_data.append(QString("MAINTAIN_CHECK_RS: %1H (维护指令)\r\n").arg(Sl_data.at(offset++)));
    output_data.append(QString("ARM编号: %1H\r\n").arg(Sl_data.at(offset++)));
    temp=offset;
    for(;offset<temp+10;offset++){
        VersionArr=VersionArr.append(Sl_data.at(offset));
    }
    QByteArray text = QByteArray::fromHex(VersionArr);
    output_data.append(QString("版本号: %1\r\n").arg(QString(text)));
    output_data.append(QString("内网IP: %1.%2.%3.%4\r\n").arg(Sl_data.at(offset).toUInt(NULL,16)).arg(Sl_data.at(offset+1).toUInt(NULL,16)).arg(Sl_data.at(offset+2).toUInt(NULL,16)).arg(Sl_data.at(offset+3).toUInt(NULL,16)));
    offset+=4;
    output_data.append(QString("外网IP: %1.%2.%3.%4\r\n").arg(Sl_data.at(offset).toUInt(NULL,16)).arg(Sl_data.at(offset+1).toUInt(NULL,16)).arg(Sl_data.at(offset+2).toUInt(NULL,16)).arg(Sl_data.at(offset+3).toUInt(NULL,16)));
    offset+=4;
    output_data.append(QString("端口: %1%2\r\n").arg(Sl_data.at(offset).toUInt(NULL,16)).arg(Sl_data.at(offset+1).toUInt(NULL,16)));
    offset+=2;
    output_data.append(QString("子网掩码: %1.%2.%3.%4\r\n").arg(Sl_data.at(offset).toUInt(NULL,16)).arg(Sl_data.at(offset+1).toUInt(NULL,16)).arg(Sl_data.at(offset+2).toUInt(NULL,16)).arg(Sl_data.at(offset+3).toUInt(NULL,16)));
    offset+=4;
    output_data.append(QString("网关: %1.%2.%3.%4\r\n").arg(Sl_data.at(offset).toUInt(NULL,16)).arg(Sl_data.at(offset+1).toUInt(NULL,16)).arg(Sl_data.at(offset+2).toUInt(NULL,16)).arg(Sl_data.at(offset+3).toUInt(NULL,16)));
    offset+=4;
    output_data.append(QString("硬盘使用情况: %1\r\n").arg((Sl_data.at(offset++).toUInt(NULL,16))));
    output_data.append(QString("信道号: %1\r\n").arg((Sl_data.at(offset++).toUInt(NULL,16))));
    output_data.append(QString("bst间隔: %1\r\n").arg((Sl_data.at(offset++).toUInt(NULL,16))));
    output_data.append(QString("分时总长度: %1\r\n").arg((Sl_data.at(offset++).toUInt(NULL,16))));
    output_data.append(QString("功率: %1\r\n").arg((Sl_data.at(offset++).toUInt(NULL,16))));
    output_data.append(QString("PCI信息: %1 (是否支持PCI，0：不支持，1：支持)\r\n").arg((Sl_data.at(offset++).toUInt(NULL,16))));
    quint8 AntaNum=Sl_data.at(offset++).toUInt(NULL,16);
    output_data.append(QString("天线数量: %1\r\n\r\n").arg(AntaNum));
    for(quint8 i=0;i<AntaNum;i++){
        for(quint8 j=0;j<10;j++){
            ant_versionArr+=Sl_data.at(offset++);
        }
        QByteArray text2 = QByteArray::fromHex(ant_versionArr);
        output_data.append("天线盒基础信息如下:\r\n");
        output_data.append(QString("天线%1版本: %2\r\n").arg(i).arg(QString(text2)));
        ant_versionArr.clear();
        output_data.append(QString("车道号: %1\r\n").arg((Sl_data.at(offset++)).toUInt(NULL,16)));
        output_data.append(QString("分时次序: %1\r\n").arg((Sl_data.at(offset++)).toUInt(NULL,16)));
        output_data.append(QString("功率: %1\r\n").arg((Sl_data.at(offset++)).toUInt(NULL,16)));
        output_data.append(QString("天线开关情况: %1 （1:开；0:关）\r\n").arg((Sl_data.at(offset++)).toUInt(NULL,16)));
        psam1status=Sl_data.at(offset++).toUInt(NULL,16);
        psam2status=Sl_data.at(offset++).toUInt(NULL,16);
        output_data.append(QString("PSAM1_STATUS: %1 (%2)\r\n").arg(psam1status).arg(dealRSUPSAMStatus(psam1status)));
        output_data.append(QString("PSAM2_STATUS: %1 (%2)\r\n\r\n").arg(psam2status).arg(dealRSUPSAMStatus(psam2status)));
    }
    temp=offset;
    for(;offset<temp+10;offset++){
        datestr=datestr.append(Sl_data.at(offset));
    }
    output_data.append(QString("主控盒控制器验证码: %1\r\n\r\n").arg(QString(QByteArray::fromHex(datestr))));
    for(int i=0;i<AntaNum;i++){
        output_data.append(QString("天线盒%1控制器验证码: ").arg(i));
        output_data.append(Sl_data.at(offset)+Sl_data.at(offset+1)+Sl_data.at(offset+2)+Sl_data.at(offset+3)+"\r\n");
        offset+=4;
        output_data.append("同步状态: "+Sl_data.at(offset)+Sl_data.at(offset+1)+Sl_data.at(offset+2)+Sl_data.at(offset+3)+"\r\n\r\n");
        offset+=4;
    }
}

void MainWindow::dealcheck2(QString input_data)
{

    int offset = 0;
    int temp = 0;
    QByteArray Version_arr;
    QByteArray datastr=input_data.toLatin1();
    output_data.append(QString("MAINTAIN_CHECK_RS: %1H (维护指令)\r\n").arg(datastr[offset++]));
    output_data.append(QString("ARM编号: %1H \r\n").arg(datastr[offset++]));
    temp=offset;
    for(;offset<temp+10;offset++){
        Version_arr=Version_arr.append(datastr.at(offset));
    }
    output_data.append("ARM编号: "+QByteArray::fromHex(Version_arr)+"\r\n");
    qDebug()<<Version_arr;
    qDebug()<<QByteArray::fromHex(Version_arr);
}

QString MainWindow::dealB0StatusCode(quint8 Status)
{
    QString StatusStr;
    switch (Status) {
    case 0x00:
        StatusStr="路侧单元状态正常";
        break;
    case 0x01:
        StatusStr="PSAM 复位失败（由路侧单元自行确认，影响本门架正常交易时赋值）";
        break;
    case 0x02:
        StatusStr="路侧单元天线头连接失败（由路侧单元自行确认，影响本门架正常交易时赋值）";
        break;
    case 0x03:
        StatusStr="路侧单元控制器初始化异常";
        break;
    default:
        StatusStr="非法数值";
        break;
    }
    return StatusStr;
}

QString MainWindow::dealB2StatusCode(quint8 DeviceType,quint8 Status)
{
    QString StatusStr;
    switch (DeviceType) {
    case 0x01:
        if((Status&0x80)==0x00){
            StatusStr="CPU 用户卡存在";
        }
        else{
            StatusStr="CPU 用户卡不存在";
        }
        if((Status&0x20)==0x00){
            StatusStr.append(",接触式界面");
        }
        else{
            StatusStr.append(",非接触式界面");
        }
        if((Status&0x08)==0x00){
            StatusStr.append(",CPU 用户卡正常");
        }
        else{
            StatusStr.append(",CPU 用户卡出错");
        }
        if((Status&0x04)==0x00){
            StatusStr.append(",OBU 未锁");
        }
        else{
            StatusStr.append(",OBU 被锁");
        }
        if((Status&0x02)==0x00){
            StatusStr.append(",OBU 未被拆动");
        }
        else{
            StatusStr.append(",OBU 被拆动");
        }
        if((Status&0x01)==0x00){
            StatusStr.append(",OBU 电池正常");
        }
        else{
            StatusStr.append(",OBU 电池电量低");
        }
        break;

    case 0x02:
        StatusStr="CPU 用户卡存在";
        if((Status&0x04)==0x00){
            StatusStr.append(",OBU 未锁");
        }
        else{
            StatusStr.append(",OBU 被锁");
        }
        if((Status&0x02)==0x00){
            StatusStr.append(",OBU 未被拆动");
        }
        else{
            StatusStr.append(",OBU 被拆动");
        }
        if((Status&0x01)==0x00){
            StatusStr.append(",OBU 电池正常");
        }
        else{
            StatusStr.append(",OBU 电池电量低");
        }
        break;

    case 0x03:
        if((Status&0x80)==0x00){
            StatusStr="CPC 卡电池正常";
        }
        else{
            StatusStr="CPC 卡电池电量低";
        }
        break;

    default:
        StatusStr="非法数据";
        break;
    }


    return StatusStr;
}

QString MainWindow::dealB4ErrorCode(quint8 ErrorCode)
{
    QString ErrorCodeStr;
    switch (ErrorCode) {
    case 0x00:
        ErrorCodeStr="本次交易成功执行，后续各项信息均合法有效";
        break;
    case 0x01:
        ErrorCodeStr="无 DSRC 数据返回（超时）";
        break;
    case 0x02:
        ErrorCodeStr="上行 DSRC 数据解码错误";
        break;
    case 0x03:
        ErrorCodeStr="路侧单元天线头连接失败（由路侧单元自行确认，影响本门架正常交易时赋值）";
        break;
    case 0x04:
        ErrorCodeStr="弃用， 保留";
        break;
    case 0x05:
        ErrorCodeStr="解密 OBU 车辆信息失败（PSAM 卡解密失败）";
        break;
    case 0x06:
        ErrorCodeStr="解密 OBU 车辆信息失败（校验码对比失败）";
        break;
    case 0x07:
        ErrorCodeStr="弃用， 保留";
        break;
    case 0x08:
        ErrorCodeStr="弃用， 保留";
        break;
    case 0x09:
        ErrorCodeStr="弃用， 保留";
        break;
    case 0x0A:
        ErrorCodeStr="OBU 读取入/出口信息文件返回数据非 9000H（单片式 OBU）";
        break;
    case 0x0B:
        ErrorCodeStr="CPC 卡读取 DF01\\EF02 文件返回数据非 9000H";
        break;
    case 0x0C:
        ErrorCodeStr="CPC 卡读取 DF01\\EF04 文件返回数据非 9000H";
        break;
    case 0x0D:
        ErrorCodeStr="CPC 卡外部认证失败（PSAM 返回）";
        break;
    case 0x0E:
        ErrorCodeStr="CPC 卡外部认证失败（CPC 卡返回）";
        break;
    case 0x10:
        ErrorCodeStr="弃用，保留";
        break;
    case 0x11:
        ErrorCodeStr="标签无卡、 选择/读取 EF04 文件成功（双片式 OBU）";
        break;
    case 0x12:
        ErrorCodeStr="标签无卡、 选择/读取 EF04 文件失败（双片式 OBU）";
        break;
    case 0x13:
        ErrorCodeStr="标签有卡、 读取 ETC 卡片文件成功，选择/读取 EF04 文件失败（双片式 OBU）";
        break;
    case 0x14:
        ErrorCodeStr="标签有卡、 读取 ETC 卡片文件失败， 选择/读取 EF04 文件失败（双片式 OBU）";
        break;
    case 0x15:
        ErrorCodeStr="标签有卡、 读取 ETC 卡片文件失败，选择/读取 EF04 文件成功（双片式 OBU）";
        break;
    case 0xFF:
        ErrorCodeStr="测试数据帧";
        break;
    default:
        ErrorCodeStr="非法数值";
        break;
    }
    return ErrorCodeStr;
}

QString MainWindow::dealB4ErrorCode35(quint8 ErrorCode)
{
    QString ErrorCodeStr;
    switch (ErrorCode) {
    case 0x00:
        ErrorCodeStr="本次交易成功执行，后续各项信息均合法有效";
        break;
    case 0x01:
        ErrorCodeStr="无 DSRC 数据返回（超时）";
        break;
    case 0x02:
        ErrorCodeStr="上行 DSRC 数据解码错误";
        break;
    case 0x03:
        ErrorCodeStr="路侧单元天线头连接失败（由路侧单元自行确认，影响本门架正常交易时赋值）";
        break;
    case 0x04:
        ErrorCodeStr="弃用， 保留";
        break;
    case 0x05:
        ErrorCodeStr="解密 双片式OBU 车辆信息失败（PSAM 卡解密失败）";
        break;
    case 0x06:
        ErrorCodeStr="解密 双片式OBU 车辆信息失败（校验码对比失败）";
        break;
    case 0x07:
        ErrorCodeStr="单片式 OBU： 读取车辆信息失败";
        break;
    case 0x08:
        ErrorCodeStr="单片式 OBU： 读取 DF01\\EF02 文件失败";
        break;
    case 0x09:
        ErrorCodeStr="单片式 OBU： 读取随机数失败";
        break;
    case 0x0A:
        ErrorCodeStr="单片式 OBU： 读取 DF01\\EF07 文件失败";
        break;
    case 0x0B:
        ErrorCodeStr="CPC 卡： 读取 DF01\\EF02 文件失败";
        break;
    case 0x0C:
        ErrorCodeStr="弃用，保留";
        break;
    case 0x0D:
        ErrorCodeStr="外部认证码计算失败（PSAM 返回";
        break;
    case 0x0E:
        ErrorCodeStr="外部认证失败（CPC 卡/单片式 OBU 返回）";
        break;
    case 0x10:
        ErrorCodeStr="弃用，保留";
        break;
    case 0x11:
        ErrorCodeStr="双片式 OBU： 标签无卡、选择/读取 EF04 文件成功";
        break;
    case 0x12:
        ErrorCodeStr="双片式 OBU： 标签无卡、 选择/读取 EF04 文件失败";
        break;
    case 0x13:
        ErrorCodeStr="双片式 OBU： 标签有卡、 读取 ETC 卡片文件成功，选择/读取 EF04 文件失败";
        break;
    case 0x14:
        ErrorCodeStr="双片式 OBU： 标签有卡、 读取 ETC 卡片文件失败， 选择/读取 EF04 文件失败";
        break;
    case 0x15:
        ErrorCodeStr="双片式 OBU： 标签有卡、 读取 ETC 卡片文件失败，选择/读取 EF04 文件成功";
        break;
    case 0xFF:
        ErrorCodeStr="测试数据帧";
        break;
    default:
        ErrorCodeStr="非法数值";
        break;
    }
    return ErrorCodeStr;
}

QString MainWindow::dealB5ErrorCode(quint8 ErrorCode)
{
    QString ErrorCodeStr;
    switch (ErrorCode) {
    case 0x00:
        ErrorCodeStr="本次交易成功执行，后续各项信息均合法有效";
        break;
    case 0x01:
        ErrorCodeStr="无 DSRC 数据返回（复合消费 DSRC 数据未收到回复，多排门架时后续门架软件应进行逻辑判断是否需要尝试重取 TAC 或更新 EF04 文件）";
        break;
    case 0x02:
        ErrorCodeStr="上行 DSRC 数据解码错误";
        break;
    case 0x03:
        ErrorCodeStr="弃用， 保留";
        break;
    case 0x04:
        ErrorCodeStr="弃用， 保留";
        break;
    case 0x05:
        ErrorCodeStr="弃用， 保留";
        break;
    case 0x06:
        ErrorCodeStr="PSAM 卡 MAC1 计算失败（双片式 OBU）";
        break;
    case 0x07:
        ErrorCodeStr="MAC2 校验失败（双片式 OBU）";
        break;
    case 0x08:
        ErrorCodeStr="计算 TAC 失败（双片式 OBU、单片式 OBU）";
        break;
    case 0x09:
        ErrorCodeStr="OBU 外部认证失败（单片式 OBU）";
        break;
    case 0x0A:
        ErrorCodeStr="OBU 更新交易记录文件返回数据非 9000H（单片式 OBU）";
        break;
    case 0x0B:
        ErrorCodeStr="CPC 卡外部认证失败";
        break;
    case 0x0C:
        ErrorCodeStr="CPC 卡写过站信息返回数据非 9000H， 写计费信息成功";
        break;
    case 0x0D:
        ErrorCodeStr="CPC 卡写过站信息成功， 写计费信息返回数据非 9000H";
        break;
    case 0x0E:
        ErrorCodeStr="CPC 卡写过站信息、 写计费信息返回数据非 9000H";
        break;
    case 0x0F:
        ErrorCodeStr="CPC 卡写计费信息返回数据非 9000H（省界出口 ETC 门架系统时）";
        break;
    case 0x10:
        ErrorCodeStr="更新 EF04 文件失败， 针对 C6 帧只更新 EF04（双片式 OBU）";
        break;
    case 0x11:
        ErrorCodeStr="复合消费失败， 针对 C6 帧只进行复合消费（双片式 OBU）";
        break;
    case 0x12:
        ErrorCodeStr="更新 EF04 文件成功、 复合消费失败（双片式 OBU）";
        break;
    case 0x13:
        ErrorCodeStr="更新 EF04 文件失败、复合消费失败（双片式 OBU）";
        break;
    case 0x14:
        ErrorCodeStr="更新 EF04 文件失败、复合消费成功（双片式 OBU）";
        break;
    case 0xFF:
        ErrorCodeStr="测试数据帧";
        break;
    default:
        ErrorCodeStr="非法数值";
        break;
    }
    return ErrorCodeStr;
}

QString MainWindow::dealB7ErrorCode(quint8 ErrorCode)
{
    QString ErrorCodeStr;
    switch (ErrorCode) {
    case 0x00:
        ErrorCodeStr="本次交易成功执行，后续各项信息均合法有效";
        break;
    case 0x01:
        ErrorCodeStr="无 DSRC 数据返回";
        break;
    case 0x02:
        ErrorCodeStr="上行 DSRC 数据解码错误";
        break;
    case 0x03:
        ErrorCodeStr="CPC 卡读取过站信息文件（DF01\EF02）返回数据非 9000H";
        break;
    case 0x04:
        ErrorCodeStr="CPC 卡更新过站信息文件（DF01\EF02）返回数据非 9000H";
        break;
    case 0x05:
        ErrorCodeStr="PSAM 卡外部认证失败";
        break;
    case 0xFF:
        ErrorCodeStr="测试数据帧";
        break;
    default:
        ErrorCodeStr="非法数值";
        break;
    }
    return ErrorCodeStr;
}

QString MainWindow::dealB8ErrorCode(quint8 ErrorCode)
{
    QString ErrorCodeStr;
    switch (ErrorCode) {
    case 0x00:
        ErrorCodeStr="本次交易成功执行，后续各项信息均合法有效";
        break;
    case 0x01:
        ErrorCodeStr="无 DSRC 数据返回";
        break;
    case 0x02:
        ErrorCodeStr="上行 DSRC 数据解码错误";
        break;
    case 0x03:
        ErrorCodeStr="OBU 更新 EF04 文件成功、 获取 TAC 码失败";
        break;
    case 0x04:
        ErrorCodeStr="OBU 更新 EF04 文件失败（含 C8 指示不需要更新 EF04 文件）、 获取 TAC失败";
        break;
    case 0x05:
        ErrorCodeStr="OBU 更新 EF04 文件失败、 获取 TAC 成功";
        break;
    case 0xFF:
        ErrorCodeStr="测试数据帧";
        break;
    default:
        ErrorCodeStr="非法数值";
        break;
    }
    return ErrorCodeStr;
}

QString MainWindow::dealBAErrorCode(quint8 ErrorCode)
{
    QString ErrorCodeStr;
    switch (ErrorCode) {
    case 0x00:
        ErrorCodeStr="本次操作执行成功，后续各项信息均合法有效";
        break;

    case 0x01:
        ErrorCodeStr="上电初始化失败";
        break;

    default:
        ErrorCodeStr="ErrorCode数据非法";
        break;
    }
    return ErrorCodeStr;
}

QString MainWindow::dealBBErrorCode(quint8 ErrorCode)
{
    QString ErrorCodeStr;
    switch (ErrorCode) {
    case 0x00:
        ErrorCodeStr="本次操作执行成功，后续各项信息均合法有效";
        break;

    case 0x01:
        ErrorCodeStr="在线授权失败";
        break;

    default:
        ErrorCodeStr="ErrorCode数据非法";
        break;
    }
    return ErrorCodeStr;
}

QString MainWindow::dealRSUControlStatus(quint8 RSUControlStatus)
{
    QString StatusStr;
    switch (RSUControlStatus) {
    case 0x00:
        StatusStr="主机+正常";
        break;
    case 0x01:
        StatusStr="主机+异常";
        break;
    case 0x02:
        StatusStr="从机+正常";
        break;
    case 0x03:
        StatusStr="从机+异常";
        break;
    default:
        break;
    }
    return StatusStr;
}

QString MainWindow::dealRSUPSAMStatus(quint8 PsamStatus)
{
    QString PsamStatustr;
    switch (PsamStatus) {
    case 0x00:
        PsamStatustr="NO CARD";
        break;
    case 0x01:
        PsamStatustr="IDLE";
        break;
    case 0x02:
        PsamStatustr="BUSY";
        break;
    case 0x03:
        PsamStatustr="UNAUTHEN";
        break;
    case 0x04:
        PsamStatustr="AUTHEN GET RANDOM";
        break;
    case 0x05:
        PsamStatustr="AUTHENING";
        break;
    case 0x06:
        PsamStatustr="PSAM鉴权失败";
        break;
    case 0xfd:
        PsamStatustr="PSAM锁卡";
        break;
    case 0xfe:
        PsamStatustr="PSAM保护状态";
        break;
    case 0xff:
        PsamStatustr="PSAM异常";
        break;

    default:
        PsamStatustr="异常值";
        break;
    }
    return PsamStatustr;
}

void MainWindow::on_pushButton_clicked()
{
    QString Vctitle="VC帧解析工具 V0.0.3  Made by Alan";
    QString helpstr="解析格式：\r\nCX帧如：C13ECE2F41BDADCBD5BDADCBD5      BX帧如：\r\nFFFF00020000001CB100110102010001020000020100010200000202010000160200001670CF\r\n";
    helpstr.append("支持ETC门架系统PC-RSU接口 V3.5 V3.4 V3.1版协议；支持一键检查维护帧解析；支持DSRC帧解析\r\n\r\n");
    helpstr.append("Copyright 2020-2029 The CGTECH Company Ltd. All rights reserved. www.cgtech.com\r\n");
    helpstr.append("The program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.");
    QMessageBox::about(this, Vctitle, helpstr);
}

QString MainWindow::dealDSRC(QStringList* data){
    QString errstr;
    if(data->at(7)=="91"){
       if(data->at(8)=="C0"){
           dealBST(data);
       }
       else if(data->at(8)=="D0"){
           dealVST(data);
       }
       else if(data->at(10)=="1B"){
           dealSetMMI_rs(data);
       }
       else if(data->at(10)=="19"){
           data->insert(0,"00");
           dealTransferChannel_rs(data);
       }
       else if(data->at(10)=="15"){
           data->insert(0,"00");
           dealGetSecure_rs(data);
       }
       else{
           errstr="帧格式错误";
       }
    }
    else if(data->at(6)=="91"){
        if(data->at(7)=="C0"){
            dealBST(data);
        }
        else if(data->at(7)=="D0"){
            data->insert(0,"00");
            dealVST(data);
        }
        else if(data->at(10)=="1A"){
            dealSetMMI_rq(data);
        }
        else if(data->at(7)=="60"){
            dealEventReport_rq(data);
        }
        else if(data->at(10)=="18"){
            data->insert(0,"80");
            data->insert(1,"01");
            data->insert(2,"00");
            data->insert(3,"20");
            dealTransferChannel_rq(data);
        }
        else if(data->at(10)=="14"){
            data->insert(0,"80");
            data->insert(1,"01");
            data->insert(2,"00");
            data->insert(3,"20");
            dealGetSecure_rq(data);
        }
        else{
            errstr="帧格式错误";
        }
    }
    else if(data->at(10)=="91"){
        if(data->at(14)=="14"){
            dealGetSecure_rq(data);
            qDebug()<<"dealGetSecure_rq";
        }
        else if(data->at(14)=="18"){
            dealTransferChannel_rq(data);
        }
        else{
            errstr="帧格式错误";
        }
    }
    else if(data->at(8)=="91"){
        if(data->at(11)=="15"){
            dealGetSecure_rs(data);
        }
        else if(data->at(11)=="19"){
            dealTransferChannel_rs(data);
        }
        else{
            errstr="帧格式错误";
        }
    }

    else{
        errstr="帧格式错误";
    }
    return errstr;
}

void MainWindow::dealBST(QStringList* data){
    quint16 datasize=data->size();
    if(datasize<25){
        return;
    }
    QString unixtimeStr=data->at(12)+data->at(12)+data->at(14)+data->at(15);
    uint time = unixtimeStr.toUInt(NULL,16);
    QDateTime datatime = QDateTime::fromTime_t(time);
    quint8 fileread=data->at(21).toUInt(NULL,16);
    output_data.append("帧类型：INITIALISATION_request（BST）\r\n");
    output_data.append("MAC地址: "+data->at(0)+data->at(1)+data->at(2)+data->at(3)+"\r\n");
    output_data.append("MAC控制域: "+data->at(4)+"\r\n");
    output_data.append("LLC控制域: "+data->at(5)+"\r\n");
    output_data.append("字段字头: "+data->at(6)+"\r\n");
    output_data.append("T-APDU&Optional: "+data->at(7)+" (BST)\r\n");
    output_data.append("Rsu BeaconID: "+data->at(8)+data->at(9)+data->at(10)+data->at(11)+"\r\n");
    output_data.append("UNIXTime: "+data->at(12)+data->at(12)+data->at(14)+data->at(15)+" (");
    output_data.append(datatime.toString("yyyy-MM-dd hh:mm:ss:zzz ddd")+")\r\n");
    output_data.append("Profile: "+data->at(16)+" ( 信道号，00H:A类,通道 1；01H:A 类,通道 2；10H:B类,通道 1；11H：B 类,通道 2)\r\n");
    output_data.append("MandApplications: "+data->at(17)+" \r\n");
    output_data.append("DsrcApplicationEntityID: "+data->at(18)+" \r\n");
    output_data.append("Icctransmode: "+data->at(19)+" (RSU 支持的卡片交易模式)\r\n");
    output_data.append("PretreatPara: "+data->at(20)+" (Container Type=0x29)\r\n");
    output_data.append("Option indicator: "+data->at(21)+" \r\n");
    output_data.append("sysinfoFileMode: "+data->at(22)+" (系统信息文件预处理模式)\r\n");
    quint8 offset=23;
    if((fileread&0x80)==0x80){
        output_data.append("Length0002: "+data->at(offset)+data->at(offset+1)+" (预读 0002 偏移量和长度)\r\n");
        offset+=2;
    }
    else{
        output_data.append("Length0002: NULL (预读 0002 偏移量和长度)\r\n");
    }
    if((fileread&0x40)==0x40){
        output_data.append("Length0012: "+data->at(offset)+data->at(offset+1)+" (预读 0012 偏移量和长度)\r\n");
        offset+=2;
    }
    else{
        output_data.append("Length0012: NULL (预读 0012 偏移量和长度)\r\n");
    }
    if((fileread&0x20)==0x20){
        output_data.append("Length0015: "+data->at(offset)+data->at(offset+1)+" (预读 0015 偏移量和长度)\r\n");
        offset+=2;
    }
    else{
        output_data.append("Length0015: NULL (预读 0015 偏移量和长度)\r\n");
    }
    if((fileread&0x10)==0x10){
        output_data.append("Length0019: "+data->at(offset)+data->at(offset+1)+" (预读 0019 偏移量和长度)\r\n");
        offset+=2;
    }
    else{
        output_data.append("Length0019: NULL (预读 0019 偏移量和长度)\r\n");
    }
    output_data.append("Profilelist: "+data->at(datasize-3)+" \r\n");
    output_data.append("FCS: "+data->at(datasize-2)+data->at(datasize-1)+" (帧校验)\r\n");
}

void MainWindow::dealVST(QStringList* data){
    QString ICCInfoStr;
    quint16 datasize=data->size();
    quint8 status=data->at(datasize-4).toUInt(NULL,16);
    if(datasize<51){
        return;
    }
    output_data.append("帧类型：INITIALISATION_request（VST）\r\n");
    output_data.append("OBU/CPC MAC: "+data->at(1)+data->at(2)+data->at(3)+data->at(4)+"\r\n");
    output_data.append("MAC控制域: "+data->at(5)+"\r\n");
    output_data.append("LLC控制域: "+data->at(6)+"\r\n");
    output_data.append("字段字头: "+data->at(7)+"\r\n");
    output_data.append("T-APDU&Optional: "+data->at(8)+" (VST)\r\n");
    output_data.append("Profile: "+data->at(9)+" ( 信道号，00H:A类,通道 1；01H:A 类,通道 2；10H:B类,通道 1；11H：B 类,通道 2)\r\n");
    output_data.append("ApplicationList: "+data->at(10)+" ( 应用列表数，至少为 1)\r\n");
    output_data.append("Aid: "+data->at(11)+" \r\n");
    output_data.append("Did: "+data->at(12)+" (1 号目录位 ETC 应用目录)\r\n");
    output_data.append("Option indicator: "+data->at(13)+"\r\n");
    output_data.append("ParameterSysInfoFile: "+data->at(14)+" ( 携带文件:系统信息文件SysInfoFile)\r\n");
    output_data.append("服务提供商编码: "+data->at(15)+data->at(16)+data->at(17)+data->at(18)+data->at(19)+data->at(20)+data->at(21)+data->at(22)+"\r\n");
    output_data.append("ContractType: "+data->at(23)+" (协约类型)\r\n");
    output_data.append("ContractVersion: "+data->at(24)+" (协约版本)\r\n");
    output_data.append("ContractSerialNumber: "+data->at(25)+data->at(26)+data->at(27)+data->at(28)+data->at(29)+data->at(30)+data->at(31)+data->at(32)+" (合同序列号)\r\n");
    output_data.append("ContractSignedDate: "+data->at(33)+data->at(34)+data->at(35)+data->at(36)+" (合同签署日期)\r\n");
    output_data.append("ContractExpiredDate: "+data->at(37)+data->at(38)+data->at(39)+data->at(40)+" (合同过期日期)\r\n");
    output_data.append("Parameter.gbICCInfo: "+data->at(41)+"\r\n");
    if(datasize!=50){
        for(quint16 i=42;i<(datasize-9);i++){
            ICCInfoStr=ICCInfoStr.append(data->at(i));
        }
    }
    output_data.append("ICCInfo: "+ICCInfoStr+" (IC 卡预读信息)\r\n");
    output_data.append("ObuConfiguration: "+data->at(datasize-9)+data->at(datasize-8)+data->at(datasize-7)+data->at(datasize-6)+"\r\n");
    output_data.append("EquipmentClass_Version: "+data->at(datasize-5)+" ( IC 卡接口、硬件版本)\r\n");
    output_data.append("OBUStatus: "+data->at(datasize-4)+" ("+dealB2StatusCode(1,status)+")\r\n");
    output_data.append("拆卸状态: "+data->at(datasize-3)+" (ESAM 系统信息文件第27 字节)\r\n");
    output_data.append("FCS: "+data->at(datasize-2)+data->at(datasize-1)+" (帧校验)\r\n");
}

void MainWindow::dealGetSecure_rq(QStringList* data){
    quint8 accessCredentialsOp=(data->at(11).toUInt(NULL,16))&&0x08;
    quint8 datasize=data->size();
    if(datasize<32){
        return;
    }
    output_data.append("帧类型：GetSecure.request\r\n");
    output_data.append("OBU/CPC MAC: "+data->at(4)+data->at(5)+data->at(6)+data->at(7)+"\r\n");
    output_data.append("MAC控制域: "+data->at(8)+"\r\n");
    output_data.append("LLC控制域: "+data->at(9)+"\r\n");
    output_data.append("字段字头: "+data->at(10)+"\r\n");
    output_data.append("T-APDU&Optional: "+data->at(11)+"\r\n");
    output_data.append("Did: "+data->at(12)+" \r\n");
    output_data.append("ActionType: "+data->at(13)+" (GetSecure ActionType=0)\r\n");
    if(accessCredentialsOp==0x08){
        output_data.append("accessCredentials: "+data->at(14)+data->at(15)+data->at(16)+data->at(17)+data->at(18)+data->at(19)+data->at(20)+data->at(21)+" \r\n");
    }
    output_data.append("GetSecureRq: "+data->at(datasize-18)+" (Container=0x14)\r\n");
    output_data.append("KeyIDforEncryptOP: "+data->at(datasize-17)+"\r\n");
    output_data.append("FID: "+data->at(datasize-16)+"\r\n");
    output_data.append("offset: "+data->at(datasize-15)+data->at(datasize-14)+"\r\n");
    output_data.append("Length: "+data->at(datasize-13)+"\r\n");
    output_data.append("rndRSU: "+data->at(datasize-12)+data->at(datasize-11)+data->at(datasize-10)+data->at(datasize-9)+data->at(datasize-8)+data->at(datasize-7)+data->at(datasize-6)+data->at(datasize-5)+"\r\n");
    output_data.append("KeyIDforAuthen: "+data->at(datasize-4)+"\r\n");
    output_data.append("KeyIDforEncrypt: "+data->at(datasize-3)+"\r\n");
    output_data.append("FCS: "+data->at(datasize-2)+data->at(datasize-1)+"\r\n");
}

void MainWindow::dealGetSecure_rs(QStringList* data){
    QString filestr;
    quint8 datasize=data->size();
    if(datasize<21){
        return;
    }
    output_data.append("帧类型：GetSecure.response\r\n");
    output_data.append("OBU/CPC MAC: "+data->at(1)+data->at(2)+data->at(3)+data->at(4)+"\r\n");
    output_data.append("MAC控制域: "+data->at(5)+"\r\n");
    output_data.append("LLC控制域: "+data->at(6)+"\r\n");
    output_data.append("状态子域: "+data->at(7)+"\r\n");
    output_data.append("字段字头: "+data->at(8)+"\r\n");
    output_data.append("T-APDU&Optional: "+data->at(9)+"\r\n");
    output_data.append("DID: "+data->at(10)+" \r\n");
    output_data.append("GetSecureRs: "+data->at(11)+" (Container=0x15)\r\n");
    output_data.append("FID: "+data->at(12)+" \r\n");
    for(quint8 i=13;i<(datasize-11);i++){
        filestr.append(data->at(i));
    }
    output_data.append("file: "+filestr+" (车辆信息文件+鉴别码的密文)\r\n");
    output_data.append("Authenticator: "+data->at(datasize-11)+data->at(datasize-10)+data->at(datasize-9)+data->at(datasize-8)+data->at(datasize-7)+data->at(datasize-6)+data->at(datasize-5)+data->at(datasize-4)+"\r\n");
    output_data.append("ReturnStatus: "+data->at(datasize-3)+" (OBU 处理状态)\r\n");
    output_data.append("FCS: "+data->at(datasize-2)+data->at(datasize-1)+"\r\n");
}

void MainWindow::dealTransferChannel_rq(QStringList* data){
    QString apdustr;
    quint8 datasize=data->size();
    if(datasize<17){
        return;
    }
    output_data.append("帧类型：TransferChannel.request\r\n");
    output_data.append("OBU/CPC MAC: "+data->at(4)+data->at(5)+data->at(6)+data->at(7)+"\r\n");
    output_data.append("MAC控制域: "+data->at(8)+"\r\n");
    output_data.append("LLC控制域: "+data->at(9)+"\r\n");
    output_data.append("字段字头: "+data->at(10)+"\r\n");
    output_data.append("T-APDU&Optional: "+data->at(11)+"\r\n");
    output_data.append("Did: "+data->at(12)+" \r\n");
    output_data.append("ActionType: "+data->at(13)+" (TransferChannel ActionType=3)\r\n");
    output_data.append("ChannelRq: "+data->at(14)+" \r\n");
    output_data.append("ChannelID: "+data->at(15)+" (ICC 卡通道)\r\n");
    for(quint8 i=16;i<(datasize-2);i++){
        apdustr.append(data->at(i));
    }
    output_data.append("ChannelID: "+apdustr+" (ICC 卡通道)\r\n");
    output_data.append("FCS: "+data->at(datasize-2)+data->at(datasize-1)+"\r\n");
}

void MainWindow::dealTransferChannel_rs(QStringList* data){
    QString datastr;
    quint8 datasize=data->size();
    if(datasize<21){
        return;
    }
    output_data.append("帧类型：TransferChannel.response\r\n");
    output_data.append("OBU/CPC MAC: "+data->at(1)+data->at(2)+data->at(3)+data->at(4)+"\r\n");
    output_data.append("MAC控制域: "+data->at(5)+"\r\n");
    output_data.append("LLC控制域: "+data->at(6)+"\r\n");
    output_data.append("状态子域: "+data->at(7)+"\r\n");
    output_data.append("字段字头: "+data->at(8)+"\r\n");
    output_data.append("T-APDU&Optional: "+data->at(9)+"\r\n");
    output_data.append("DID: "+data->at(10)+" \r\n");
    output_data.append("ChannelRs: "+data->at(11)+" (Container=0x19)\r\n");
    output_data.append("ChannelID: "+data->at(12)+" (IC 卡通道)\r\n");
    output_data.append("DATALIST: "+data->at(13)+" (IC 卡 APDU 指令返回的数量)\r\n");
    for(quint8 i=14;i<(datasize-3);i++){
        datastr.append(data->at(i));
    }
    output_data.append("ChannelID: "+datastr+" (IC 卡 APDU 指令返回的数据)\r\n");
    output_data.append("ReturnStatus: "+data->at(datasize-3)+" (OBU 处理状态)\r\n");
    output_data.append("FCS: "+data->at(datasize-2)+data->at(datasize-1)+"\r\n");
}

void MainWindow::dealSetMMI_rq(QStringList* data){
    output_data.append("帧类型：SetMMI.request\r\n");
    output_data.append("MAC地址: "+data->at(0)+data->at(1)+data->at(2)+data->at(3)+"\r\n");
    output_data.append("MAC控制域: "+data->at(4)+"\r\n");
    output_data.append("LLC控制域: "+data->at(5)+"\r\n");
    output_data.append("字段字头: "+data->at(6)+"\r\n");
    output_data.append("T-APDU&Optional: "+data->at(7)+" (Action.request)\r\n");
    output_data.append("DID : "+data->at(8)+" (1号目录为 ETC 应用目录)\r\n");
    output_data.append("ActionType : "+data->at(9)+" (SetMMI ActionType=4)\r\n");
    output_data.append("SetMMI.rq : "+data->at(10)+" (Container=0x1A)\r\n");
    output_data.append("Parameter : "+data->at(11)+" (0-OK 1-NOK 2…)\r\n");
    output_data.append("FCS : "+data->at(12)+data->at(13)+" (帧校验)\r\n");
}

void MainWindow::dealSetMMI_rs(QStringList* data){
    output_data.append("帧类型：SetMMI.response\r\n");
    output_data.append("MAC地址: "+data->at(0)+data->at(1)+data->at(2)+data->at(3)+"\r\n");
    output_data.append("MAC控制域: "+data->at(4)+"\r\n");
    output_data.append("LLC控制域: "+data->at(5)+"\r\n");
    output_data.append("状态子域: "+data->at(6)+"\r\n");
    output_data.append("字段字头: "+data->at(7)+"\r\n");
    output_data.append("T-APDU&Optional: "+data->at(8)+" (Action.response)\r\n");
    output_data.append("DID : "+data->at(9)+" (1号目录为 ETC 应用目录)\r\n");
    output_data.append("SetMMIRs : "+data->at(10)+" (Container=0x1B)\r\n");
    output_data.append("ReturnStatus : "+data->at(11)+" (OBU 处理状态)\r\n");
    output_data.append("FCS : "+data->at(12)+data->at(13)+" (帧校验)\r\n");
}

void MainWindow::dealEventReport_rq(QStringList* data){
    output_data.append("帧类型：Event_Report.request\r\n");
    output_data.append("MAC地址: "+data->at(0)+data->at(1)+data->at(2)+data->at(3)+"\r\n");
    output_data.append("MAC控制域: "+data->at(4)+"\r\n");
    output_data.append("LLC控制域: "+data->at(5)+"\r\n");
    output_data.append("字段字头: "+data->at(6)+"\r\n");
    output_data.append("T-APDU&Optional: "+data->at(7)+" (Event_Report.request)\r\n");
    output_data.append("DID : "+data->at(8)+" (1号目录为 ETC 应用目录)\r\n");
    output_data.append("EventType : "+data->at(9)+" (Release=0)\r\n");
    output_data.append("FCS : "+data->at(10)+data->at(11)+" (帧校验)\r\n");
}
