#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString output_data;

private slots:
    void on_btn_parse_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    QString dealVC(quint8 cmdtype, QStringList Sldata);
    //CX帧解析
    void dealC0(QStringList Sl_data);
    void dealC1(QStringList Sl_data);
    void dealC2(QStringList Sl_data);
    void dealC4(QStringList Sl_data);
    void dealC6(QStringList Sl_data);
    void dealC7(QStringList Sl_data);
    void dealC8(QStringList Sl_data);
    void dealCA(QStringList Sl_data);
    void dealCB(QStringList Sl_data);
    void dealCF(QStringList Sl_data);
    void dealFA(QStringList Sl_data);
    void dealFB(QStringList Sl_data);
    void dealFC(QStringList Sl_data);
    //BX帧解析
    void dealB0(QStringList Sl_data);
    void dealB1(QStringList Sl_data);
    void dealB2(QStringList Sl_data);
    void dealB4(QStringList Sl_data);
    void dealB5(QStringList Sl_data);
    void dealB7(QStringList Sl_data);
    void dealB8(QStringList Sl_data);
    void dealBA(QStringList Sl_data);
    void dealBB(QStringList Sl_data);
    void dealEA(QStringList Sl_data);
    void dealEB(QStringList Sl_data);
    void dealEC(QStringList Sl_data);

    void dealcheck(QStringList Sl_data);  //一键检查帧解析
    void dealcheck2(QString input_data);
    //DSRC帧解析
    QString dealDSRC(QStringList* data);
    void dealBST(QStringList* data);
    void dealVST(QStringList* data);
    void dealGetSecure_rq(QStringList* data);
    void dealGetSecure_rs(QStringList* data);
    void dealTransferChannel_rq(QStringList* data);
    void dealTransferChannel_rs(QStringList* Sl_data);
    void dealSetMMI_rq(QStringList* Sl_data);
    void dealSetMMI_rs(QStringList* Sl_data);
    void dealEventReport_rq(QStringList* Sl_data);

    QString dealB0StatusCode(quint8 Status);
    QString dealB2StatusCode(quint8 DeviceType, quint8 Status);
    QString dealB4ErrorCode(quint8 ErrorCode);
    QString dealB4ErrorCode35(quint8 ErrorCode);
    QString dealB5ErrorCode(quint8 ErrorCode);
    QString dealB7ErrorCode(quint8 ErrorCode);
    QString dealB8ErrorCode(quint8 ErrorCode);
    QString dealBAErrorCode(quint8 ErrorCode);
    QString dealBBErrorCode(quint8 ErrorCode);

    QString dealRSUControlStatus(quint8 RSUControlStatus);
    QString dealRSUPSAMStatus(quint8 PsamStatus);
    
};
// 初始化指令
typedef struct{
    quint8 cmd_type;
    uint unix_time;
    quint8 datetime[7];
    quint8 lanemode;
    quint8 bstinterval;
    quint8 txpower;
    quint8 pllchannelid;
    quint8 transmode;
    uint flagid;
    quint16 reserced;
}C0_CMD_STRTYPE;

//继续交易指令
typedef struct{
    quint8 cmd_type;
    uint obuid;
    ulong OBUDivFactor;
}C1_CMD_STRTYPE;

//停止交易指令
typedef struct{
    quint8 cmd_type;
    uint obuid;
    quint8 stoptype;
    int unix_time;
}C2_CMD_STRTYPE;

//开关路侧单元指令
typedef struct{
    quint8 cmd_type;
    quint8 ControlType;
}C4_CMD_STRTYPE;

//消费交易指令
typedef struct{
    quint8 cmd_type;
    uint obuid;
    ulong OBUDivFactor;
    quint8 OBUTradeType;
    uint ConsumeMoney;
    quint8 PurchaseTime[7];
    quint8 Station[43];
    quint8 EF04Info[91];
    quint8 EF04Info2[5];
}C6_CMD_STRTYPE;

//更新过站信息指令
typedef struct{
    quint8 cmd_type;
    int cpc_mac;
    ulong CPCDivFactor;
    quint8 UpdateType;
}C7_CMD_STRTYPE;

//重取 TAC 指令
typedef struct{
    quint8 cmd_type;
    uint obuid;
    quint16 Epserial;
    quint8 EF04CmdType;
    quint8 EF04Info[91];
    quint8 EF04Info2[5];
}C8_CMD_STRTYPE;

//指示路侧单元完成 PSAM 上电初始化功能
typedef struct{
    quint8 cmd_type;
    uint PSAMChannel;
}CA_CMD_STRTYPE;

//指示路侧单元完成 PSAM 在线授权功能
typedef struct{
    quint8 cmd_type;
    quint8 PSAMCount;
    quint8 *AuthInfo;
}CB_CMD_STRTYPE;

//心跳应答指令
typedef struct{
    quint8 cmd_type;
    quint8 Status;
}CF_CMD_STRTYPE;

//指示路侧单元进入射频信号发射测试模式
typedef struct{
    quint8 cmd_type;
    quint8 RSUID;
    quint8 TxStatus;
    quint8 TxType;
}FA_CMD_STRTYPE;

//指示路侧单元进入射频信号接收测试模式
typedef struct{
    quint8 cmd_type;
    quint8 RSUID;
    quint8 RxStatus;
    quint8 IfRevDataDiff;
}FB_CMD_STRTYPE;

//指示路侧单元进入 BeaconID 查询、 配置模式
typedef struct{
    quint8 cmd_type;
    quint8 BeaconIDStatus;
    uint BeaconID;
}FC_CMD_STRTYPE;

//设备状态信息帧
typedef struct{
    quint8 FrameType;
    quint8 RSUStatus;
    quint8 PSAMNum1;
    quint8 *PSAMInfo1;
    quint8 RSUAlgId;
    quint8 RSUManuID;
    quint8 RSUID[3];
    quint16 RSUVersion;
    quint8 workstatus;
    quint8 FlagID[3];
    uint Reserved;
}B0_CMD_STRTYPE;

//心跳信息帧
typedef struct{
    quint8 FrameType;
    quint8 RSUControlStatus1;
    quint8 RSUControlStatus2;
    quint8 RSUControlStatus3;
    quint8 PSAMNum1;
    quint8 *PSAMStatus1;
    quint8 PSAMNum2;
    quint8 *PSAMStatus2;
    quint8 RSUAntennaNum;
    quint8 RSUAntennaNum2;
    uint *AntennaStatus;
}B1_CMD_STRTYPE;

//车载单元信息帧
typedef struct{
    quint8 FrameType;
    uint OBUID_CPCMAC;
    quint8 ErrorCode;
    quint8 AntennaID;
    quint8 DeviceType;
    quint8 IssuerIdentifier[8];
    quint8 ContractType;
    quint8 ContractVersion;
    quint8 SerialNumber[8];
    quint8 DateofIssue[4];
    quint8 DateofExpire[4];
    quint8 EquitmentCV;
    quint16 OBU_CPCStatus;
}B2_CMD_STRTYPE;

//用户信息帧
typedef struct{
    quint8 FrameType;
    uint OBUID_CPCMAC;
    quint8 ErrorCode;
    quint8 TransType;
    quint8 VehicleInfo[79];
    uint CardRestMoney;
    quint8 EF04Info[91];
    quint8 IssuerInfo[50];
    quint8 LastStation[43];
    quint8 EF02_Station[4];
    quint8 EF04_CostMoney[10];
}B4_CMD_STRTYPE;

//交易信息帧
typedef struct{
    quint8 FrameType;
    uint OBUID_CPCMAC;
    quint8 ErrorCode;
    quint8 PSAMNo[6];
    quint8 TransTime[7];
    uint TransType;
    quint8 AlgFlag;
    quint8 KeyVersion;
    quint8 TAC[4];
    quint16 ICCPayserial;
    quint8 PSAMTransSerial[4];
    uint CardBalance;
}B5_CMD_STRTYPE;

// 过站信息帧
typedef struct{
    quint8 FrameType;
    uint CPCMAC;
    quint8 ErrorCode;
    quint8 WritedFlags;
    quint8 LastFalgID[3];
    quint8 *FlagID;
}B7_CMD_STRTYPE;

// TAC 信息帧
typedef struct{
    quint8 FrameType;
    uint OBUID_CPCMAC;
    quint8 ErrorCode;
    quint8 TAC[4];
    quint8 MAC[4];
    uint CardRestMoney;
}B8_CMD_STRTYPE;

//PSAM 初始化信息帧
typedef struct{
    quint8 FrameType;
    quint8 ErrorCode;
    quint8 Datetime[7];
    quint8 PSAMCount;
    quint8 *PSAMInfo;
}BA_CMD_STRTYPE;

//PSAM 授权信息帧
typedef struct{
    quint8 FrameType;
    quint8 ErrorCode;
    quint8 PSAMCount;
    quint8 *AuthResult;
}BB_CMD_STRTYPE;

//测试信号发送信息帧
typedef struct{
    quint8 FrameType;
    quint8 Status;
}EA_CMD_STRTYPE;

//测试信号接收信息帧
typedef struct{
    quint8 FrameType;
    quint8 Status;
    quint16 RevNum;
    quint8 Primitive;
    quint16 DiffRevNum;
    quint16 RevNum_CRCErr;
}EB_CMD_STRTYPE;

//BeaconID 设置信息帧
typedef struct{
    quint8 FrameType;
    quint8 Status;
    quint8 BeaconIDStatus;
    quint8 Primitive;
    quint8 BeaconID[4];
}EC_CMD_STRTYPE;

#endif // MAINWINDOW_H
