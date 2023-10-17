#ifndef APIPVIB2_0_H
#define APIPVIB2_0_H

#define U32 unsigned long
#define S32 int
#define U16 unsigned short
#define S16 short
#define U8 unsigned char

#define S64 long long

#define MaxDevNum           16
#define MaxGroupNumPerDev   16
#define MaxCardNumPerGroup  16
#define MaxChNumPerCard     64
#define MaxRangeNum         8



typedef struct _DevInf    //设备基本信息
{
	U8 DevType;     //类型码
    U8 CardNum;     //上线了的card数
	
	U16 FPGA_Ver;   //FPGA版本号
    U16 Dev_SN;    //模块编号
    U16 FuncCode;   //功能码
	U16	VIB_Ver;	//机箱软件版本
	U16 revd[2];

}DevInf;


typedef struct _SCM_Info  //调理模块基本信息 ，
{
    
	U8 SCMType;     //类型码：
    U8 FuncCode;    //功能码
    U8 ChNum;       //通道数
    U8 SlotIdx;     //槽位序号，0=第一个槽位
    U8 revd0[4]; 
	
	U16 Code_Ver;   //FPGA版本号
    U16 Card_SN;    //模块编号
    U16 DevSN;      //所属设备编号

	U16 revd1[5];
  
} SCM_Info;


typedef struct _CardBaseInfo  //卡（功能模块）基本信息
{
    
	U8 CardType;    //类型码A0: 20024；A1: 1624。。。
	U8 FuncCode;    //功能码
    U8 ChNum;       //通道数
    U8 SlotIdx;     //槽位序号，0=第一个槽位
    U8 revd0[4]; 
	
	U16 FPGA_Ver;   //FPGA版本号
    U16 Card_SN;    //模块编号
	U16 DevSN;      //所属设备编号

	U16 revd1[5];

	SCM_Info SCM0;
	SCM_Info SCM1;

  
} CardBaseInfo;


typedef struct _SCM_FuncInfo //调理模块功能信息
{
	U8   CardType;           //模块型号
	U8   Funccode;           //功能码


	U8   IcpMode;            //ICP调理功能， 0=不支持，1=支持  
	U8   StrainMode;         //应变调理功能，0=不支持，1=支持
	U8   ChargeMode;         //电荷调理功能，0=不支持，1=支持
	U8   TempMode;           //测温调理功能，0=不支持，1=支持
	U8   C_Mode;             //电流调理功能， 0=不支持，1=支持
	U8   AmpMode;            //放大调理功能，0=不支持，1=支持
	
	U8   VOutMode;           //通道供电功能，0=不支持，1=支持

    U8   DiffMode;           //单端、差分切换模式，0=不支持，1=支持
	U8	 MaxTempDivideIdx;   //热电偶挡位最大支持的挡位序号
    U8   revd0[5];

	U16  Strain_R;           //应变电阻，默认120
    U16  revd1[3];

	float V_RangeTab[8];     //电压量程挡位列表，大于0的挡位，属于有效挡位，=0的，不用显示，单位V
	float StrainTab[8];      //应变量程挡位列表，大于0的挡位，属于有效挡位，=0的，不用显示  单位με
	float ChargeTab[8];      //电荷量程挡位列表，大于0的挡位，属于有效挡位，=0的，不用显示，单位C
	float C_RangeTab[8];     //电流量程挡位列表，大于0的挡位，属于有效挡位，=0的，不用显示，单位A
	float AmpTab[8];         //增益挡位列表，大于0的挡位，属于有效挡位，=0的，不用显示

	float revd2[8];       
	float revd3[8]; 

    float StrainFactor;      //应变片系数，默认2.0
	float revd4[7];      

	float BridgeTab[8];      //桥压挡位列表，大于0的挡位，属于有效挡位，=0的，不用显示
  
	U32  LpRateTab[24];      //低通滤波挡位，大于0的挡位，属于有效挡位，=0的，不用显示
	U32  HpRateTab[8];      //高通滤波挡位，大于0的挡位，属于有效挡位，=0的，不用显示
	U32 BW; //调理模块带宽
    U32  revd5[15];

}SCM_FuncInfo;


typedef struct _CardFuncInfo //卡（功能模块）功能信息
{
	//-------------调理模块功能
    SCM_FuncInfo SCM0_Inf;
	SCM_FuncInfo SCM1_Inf;

    //-------------采集模块功能
	U8   CardType;         //模块型号
	U8   FuncCode;         //功能码
	U8   MaxClkIdxByRT;    //当前模式下，支持实时模式的最大采样率挡位，=255 表示不支持
	U8   rdvd1[5];         
	
	float AD_BaseV;        //默认2.5V
    float revd2[3];

	U32  ClkRateTab[16];   //采样率列表
	U32  MaxAcqLen;        //最大采样长度

	U32  revd3[7];
  
} CardFuncInfo;


	
typedef struct _DEV_PARM  //设备功能参数
{
	U8 ExTrgMode; //系统外触发模式，系统外触发打开后直接送上总线，0=关闭、1=系统外触发上升沿、2=系统外触发下降沿 
	U8 DaOutEn;   //设备打开DA输出 0=关闭，1=打开
	U8 DaType;    //DA输出类型，0=直流、1直流、2正弦波、3=方波
	U8 DaPeakV;   //DA输出峰值，0=1V、1=3V
	U8 DaFreq;    //DA输出信号频率，0=1K、1=10K
	U8 SaveFileFlag;//设备开启存储标志，1，开启，0关闭  
	U8 revd3[10];

}DEV_PARM;


typedef struct _CARD_PARM  //卡（功能模块）功能参数
{
	U32  AcqLen; //采集长度
	U32  DlyLen; //负延时长度
    U32  RtSaveSec; //实时存储长度，单位MB
    U32  revd1[5];
	
	U8 AcqClkIdx; //采样率挡位选择，0~15，模块采样率=CardFuncInfo.ClkRateTab[AcqClkIdx]
	U8 ClkSrc;    //时钟源选择，0=本地、1=总线、2=外部
    U8 GoSrc;     //go源选择，0=本地、1=总线、2=外部

	U8 RtEn;      //实时采集开关，0=关闭实时采集，1=打开实时采集，数据在采集同时，会自动存放在读数端口，等待零槽或上位机读数。注意，关闭RtEn后，RtSave自动关闭。
    U8 RtSave;    //实时存储开关，0=关闭实时存储，1=打开实时存储，由零槽或上位机读取实时数据，并完成存储，注意打开RtSave后，采样长度AcqLen无效(需设置2号寄存器的bit2)。打开RtSave后，RtEn应自动打开。

    U8 DlyType;   //0=负延时与触发无关，1=负延时采满再允许触发，
	U8 AuxIOOut;  //多功能IO输出信号选择， 0=go信号，1=其他
	U8 AuxIOIn;   //多功能IO输入信号选择， 0=go信号，1=其他

	U8 revd2[8];


    U8 TrgMode;   //触发模式，0=手动触发、1=外触发上升沿、2=外触发下降沿、3=内触发出窗触发、4=内触发入窗触发、5=内触发上升沿、6=内触发下降沿、7其他
	U8 TrgSrc;    //触发源选择，0=本地触发，1=总线触发
	U8 TrgOut;    //触发信号是否输出到总线，0=不输出，1=输出
	U8 TrgLogic;  //通道触发逻辑，0=或，1=与
	U8 AcqMode;  //采集模式，0=单次采集  1= 连续采集。isd设备有效

	U8 revd3[3];

	float ExClkFreq; //外时钟频率
	float revd4[3];

}CARD_PARM;




typedef struct _CH_PARM  //通道功能参数
{
	U8 ChFuncType;    //通道功能类型 0=电压直通测量，1=ICP模式，2=应变测量，3=电荷测量，4=热电偶测温，5=电流测量，6=放大器应用（不折算放大倍数）

    U8 ChInputType;   //通道输入模式选择 0=单端、1=差分；  注意，当CARD_PARM.ChFuncType=2时，必须ChInputType=1
    U8 VOutEn;        //通道电压输出 0=关闭、1=打开  ；注意，当CARD_PARM.ChFuncType=2时，必须VOut=1

	
	U8 V_RangeIdx;    //电压通道量程挡位0~7，  挡位值由SCM_FuncInfo.V_RangeTab 确定
    U8 CoupleIdx;     //通道耦合挡位0~2，  0=直流，1=交流，2=地
    U8 AmpIdx;        //通道增益挡位0~7，  挡位值由SCM_FuncInfo.AmpTab 确定
    U8 ChargeIdx;     //通道电荷调理挡位0~7，挡位值由SCM_FuncInfo.ChargeTab 确定  ,CARD_PARM.ChFuncType=3时有效
	U8 TempDivideIdx; //通道测温分度表索引，当CARD_PARM.ChFuncType=4时，有意义。0=C型、1=K型、2=J型……
	U8 C_RangeIdx;    //电流通道量程挡位0~7，  挡位值由SCM_FuncInfo.C_RangeTab 确定
	U8 BridgeIdx;     //通道桥压挡位0~3，挡位值由SCM_FuncInfo.BridgeTab 确定 ,CARD_PARM.ChFuncType=2或6时有效
    U8 StrainType;    //应变模式0~2，0=半桥邻臂，1=半桥对臂，2=1/4桥，3=全桥

	U8 LpIdx;         //低通滤波挡位，  挡位值由SCM_FuncInfo.LpRateTab 确定
    U8 HpIdx;         //高通滤波挡位，  挡位值由SCM_FuncInfo.HpRateTab 确定
    
    U8 StrainIdx;     //应变挡位
    U8 revd1;
    U8 TrgChEn;//通道内触发允许
    U16 TrgLv0;       //通道触发电平1
	U16 TrgLv1;       //通道触发电平2
	
    U16 Strain_R;     //应变电阻，默认120
	U16 revd2[5]; 
	
	U8 ChName[16];    //通道名   默认CH1、CH2……
	U8 ChUnit[8];     //通道单位  根据ChFuncIdx匹配，可以自定义
    U8 revd3[8];      //

	float ChK;        //通道K系数
	float ChOft;      //通道偏置
	float Temp_Comp;  //冷端补偿值 
	float StrainFactor;//应变片系数，默认2.0
	float StrainComp; //应变桥压补偿系数，默认4.0
	float Sensitivity;//灵敏度 (24位 dY=dY/Sensitivity)
    float revd4[2];


}CH_PARM;


typedef struct _DataHeadInfo    
{
    
	U8 CardType;                       //类型码A0: 20024；A1: 1624。。。
    U8 ChNum;                          //通道数
	U8 Resolution;                     //数据分辨率0=16位（1个样点2字节），1=24位（1个样点4字节），2=8位（1个样点1字节），其他：Resolution
	U8 revd1[5]; 
	
	U8 ChFuncType[MaxChNumPerCard];    //通道功能类型：//通道功能类型 0=电压直通测量，1=ICP模式，2=应变测量，3=电荷测量，4=热电偶测温，5=电流测量，6=放大器应用（不折算放大倍数）

	U8 TempDivideIdx[MaxChNumPerCard]; //分度表索引，当ChFuncType=4时，有意义。0=C型、1=K型……

	U8 Unit[MaxChNumPerCard][8];       //物理量单位


	S64 DataStart;                     //数据起点
    S64 DataEnd;                       //数据终点
	double DeltaX;                     //采样时间分辨率，即采样率倒数
	double DeltaY[MaxChNumPerCard];    //量化转换率，即电压转换到物理量的转换值
    double Temp_Comp[MaxChNumPerCard]; //冷端补偿值 
	double Offset[MaxChNumPerCard];    //物理量偏移
	float Gain[MaxChNumPerCard];		//数字校准
	U32 revd0[MaxChNumPerCard];
 
	U16 Card_SN;    //模块编号
	U16 revd2[3];

 }DataHeadINFO;



typedef struct _GROUP_INFO
{
    U8    CardNum;                             //组模块数
	U8    CardType;                            //模块型号
	U8    Rev[6];                              //
	U8    SCMGroupCh[MaxCardNumPerGroup];           //0=槽位的SCM0.SCM1都属于本组，1=SCM0属于本组，2=SCM1属于本组
    U32   ChNum[MaxCardNumPerGroup];           //每个元素表示该模块的通道数
	U32   CardSlotIdx[MaxCardNumPerGroup];     //每张卡的槽位号
	CardFuncInfo  FuncInfo;//组卡的功能信息  

} GROUP_INFO;

typedef struct _CH_Cal   //通道校准参数
{
    U16 AD_Gain;    //增益值
	U16 AD_Zero;    //零点值
	
	
    U16 SMC_Gain[MaxRangeNum];    //增益值
    U16 SMC_Zero[MaxRangeNum];    //零点值

}CH_Cal;

typedef struct _CH_ADCal   //通道校准参数
{
    U16 AD_Gain;    //增益值
    U16 AD_Zero;    //零点值


//	U16 SMC_Gain[MaxRangeNum];    //增益值
// 	U16 SMC_Zero[MaxRangeNum];    //零点值

}CH_ADCal;
typedef struct _CH_SMCCal   //通道校准参数
{
    U16 Apmidx;//增益挡位
    U16 rev;
    U16 SMC_Gain;    //增益值
    U16 SMC_Zero;    //零点值

}CH_SMCCal;



typedef struct _NetIp//ip地址）
{
    U8 Ip_H;    //IP地址最高位0~254
    U8 Ip_M1;    //0~254
    U8 Ip_M2;    //0~254
    U8 Ip_L;    //IP地址最低位 0~254
} NetIp;

#ifndef TDATATIMEEX
#define TDATATIMEEX
typedef struct
{
	unsigned char Year;
	unsigned char Month;
	unsigned char Date;

	unsigned char Hour;
	unsigned char Minute;
	unsigned char Second;

	unsigned short Ms; //毫秒 
	unsigned short Us; //微秒 
}TDateTimeEx;
typedef struct {
	unsigned char Deg;
	unsigned char Min;
	unsigned char Sec;
}GPSCoordin;
#endif // !TDATATIMEEX

#ifndef ISDDEVSTATUS
#define ISDDEVSTATUS
struct TPower
{
	unsigned char PowerType;   //供电类型 0：电池、1：外部供电（充电）
	unsigned char PowerV;       //电量值 1~100
};

struct TMaxV
{
	float CH0Max;
	float CH1Max;
	float CH2Max;
	float CH3Max;
};

struct ISD_DevStatus//Size::104BYTE
{
	unsigned char WorkMode; //设备当前工作模式 0：普通；1：排程
	unsigned char TrgMode;   //触发模式  0:出窗内触发；1：入窗内触发；2：上升
							 //沿内触发；3：下降沿内触发；4：外触发上	升沿；5：
							 //外触发下降沿；6:内触发+外触发7：手动触发

	unsigned char AcqStatus;  //采集状态 0：空闲；1：等待触发；2：正在采集;>200=
							  //错误码；
	unsigned char MemStatus;//剩余存储空间%
	unsigned char WifiMode; //  wifi模式,0:AP  , 1:STA
	unsigned char WifiStatus;//  wifi强度
	unsigned char GpsStatus; //0：未连接，1：已连接
	unsigned char TopCloud;  //0：未登录，1：已连接
	unsigned char LastCmd;   //上一条执行的命令地址
	unsigned char  Saveflag;      //文件存储,1=开启存储  0=关闭存储
	unsigned short CurFileNum; // 当日文件数量
	unsigned short FileNum;//当前记录的文件数量
	short temp;    //温度
	char unit[4][8];    //采集量单位
	char Type[4];    //类型
	float ch[4];   //信号频率
	TPower PowerInfo;//结构定义了供电方式和电量，正在充电的状态可以利用起来
	TMaxV CHMax;//最近一次测量，通道的最大值
	TMaxV RealData; //实时数据
	TDateTimeEx TrgTime;//最近一次测量的触发时间
	TDateTimeEx DevTime;//设备当前时间
	unsigned char Chnum;      //保留
	unsigned char CheckKeyFlag;      //保留
	unsigned char rev1[6];      //保留
};
typedef struct _QCParam
{
	TDateTimeEx TrgTime;            //本次测量触发的时间,年月日
	unsigned short QCPackNum;
	unsigned char ChIdx;
}QCParam;
typedef struct _nQCPack //Size::2112 BYTE
{
	float QC_DeltaY;                    //Y轴分辨率
	float QC_DeltaX;                    //X轴分辨率
	short Starttime;                  //数据起点
	unsigned short QCPackNum;         // 当日记录序号
	unsigned char ChIdx;                      //该包通道号
	unsigned char chnum;                      //设备当前通道数量
	unsigned char rev0[2];                        //对齐保留
	float MaxY;                     //本次测量通道的最大值
	float freq;                    //主频
	float impulse;                //冲量
	float risetime;                //上升时间
	float Revd[4];                 //保留
	TDateTimeEx TrgTime;              //本次测量触发的时间
	unsigned char rev1[6];
	short QCData[1024];
}nQCPack, *nQCPackData;
struct RemoveFile_S//
{
	TDateTimeEx trgtime;
	char filename[64];
};
#endif // !ISDDEVSTATUS

#ifndef RTFILE_DATETIME
#define RTFILE_DATETIME
typedef struct //
{
	TDateTimeEx trgtime;
	double starttime;
	double holdtime;
}RTfile_DateTime;

#endif // !RTFILE_DATETIME
#ifndef ENUM_PVIB_STATUS
#define ENUM_PVIB_STATUS

enum PVIB_STATUS :int {
	PVIB_OK = 0,//正常
	//USB模式下的错误码
	FT_INVALID_HANDLE,
	FT_DEVICE_NOT_FOUND,
	FT_DEVICE_NOT_OPENED,
	FT_IO_ERROR,
	FT_INSUFFICIENT_RESOURCES,
	FT_INVALID_PARAMETER,
	FT_INVALID_BAUD_RATE,
	FT_DEVICE_NOT_OPENED_FOR_ERASE,
	FT_DEVICE_NOT_OPENED_FOR_WRITE,
	FT_FAILED_TO_WRITE_DEVICE,
	FT_EEPROM_READ_FAILED,
	FT_EEPROM_WRITE_FAILED,
	FT_EEPROM_ERASE_FAILED,
	FT_EEPROM_NOT_PRESENT,
	FT_EEPROM_NOT_PROGRAMMED,
	FT_INVALID_ARGS,
	FT_NOT_SUPPORTED,

	FT_NO_MORE_ITEMS,
	FT_TIMEOUT,
	FT_OPERATION_ABORTED,
	FT_RESERVED_PIPE,
	FT_INVALID_CONTROL_REQUEST_DIRECTION,
	FT_INVALID_CONTROL_REQUEST_TYPE,
	FT_IO_PENDING,
	FT_IO_INCOMPLETE,
	FT_HANDLE_EOF,
	FT_BUSY,
	FT_NO_SYSTEM_RESOURCES,
	FT_DEVICE_LIST_NOT_READY,
	FT_DEVICE_NOT_CONNECTED,
	FT_INCORRECT_DEVICE_PATH,

	FT_OTHER_ERROR,
	//
	PVIB_NONSUPPORTED = 33,//不支持
	PVIB_FAILUDPPORT,//UDP端口绑定失败
	PVIB_TIMEOUT,//超时
	PVIB_SEARCHOVERRANGE,//搜索的设备数量超出数组范围
	PVIB_UNKNOWNID,//未知设备ID
	PVIB_ERRNDAQCONFIG,//读取NDAQ_CONFIGFILE.CONF错误
	PVIB_ERRSCMCONFIG,//读取SCM_CONFIGFILE.CONF错误
	PVIB_INVALIDDEVICEID,//无效的设备ID
	PVIB_ERRINIT,//初始化错误
	PVIB_ERRTCP,//TCP错误
	PVIB_ERRTCP_PROTOCOL,//TCP协议错误
	PVIB_NOTEXISTDIR,//目录不存在
	PVIB_ERRPARAM,//传入参数不正确
	PVIB_ERR_TCPRESLEN,//TCP协议返回长度不正确
	PVIB_RTDATA_NOTREADY,//实时数据未准备好
	PVIB_NOTENOUGH_STORAGE//设备储存空间不足
};
#endif

//设备信息结构体
struct DeviceInfo {
	unsigned long Id;
	unsigned char Ip[4];
	unsigned char Type;//0机箱、1模块
};

#define APIPVIB2_0_API __declspec(dllimport)
#ifdef __cplusplus
extern "C" {
#endif
/***************************SDK初始化和关闭*************************/
	// 功能:打开DLL（打开后必须在结束时调用CloseDll释放资源）
	// 返回:0正常 非0错误
    APIPVIB2_0_API PVIB_STATUS  OpenDll1(char* serverIp,unsigned short serverPort=7000);
	// 功能:打开DLL（打开后必须在结束时调用CloseDll释放资源）默认IP 127.0.0.1 端口 7000
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS  OpenDll();

	// 功能:关闭DLL释放资源
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS CloseDll();

/***************************搜索设备*************************/
	// 功能:获取设备数量和设备id信息
	// 输出:deviceIds		待输出的设备ID数组
	// 输入:arrDevConnSize	设备ID数组数量
	// 输出:searchNum		搜索结果的数量
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS GetDeviceInfo(DeviceInfo  arrDeviceInfo[], int arrSize, int &searchNum);
	
	// 功能:获取设备的分组数量
	// 输入:deviceId	设备Id
	// 输出:pGroupNum	组数量
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS GetGroupCount(unsigned long deviceId,unsigned int *pGroupNum);
	
	// 功能:获取组信息
	// 输入:deviceId	设备Id
	// 输入:groupid		组号
	// 输出:pGroupInfo	组信息
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS GetGroupInfo(unsigned long deviceId, int groupid, GROUP_INFO *pGroupInfo);

	// 功能:获取组名字
	// 输入:deviceId	设备Id
	// 输入:groupid		组号
	// 输出:name		组名字，字符串长度上限32
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS GetGroupName(unsigned long deviceId, int groupid, char* name);

/***************************参数配置*************************/
	// 功能:设置设备卡参数
	// 输入:deviceId	设备Id
	// 输入:slotIdx		槽位号
	// 输入:cardParm	卡参数
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS SetCardParm(unsigned long deviceId, int slotIdx, CARD_PARM  cardParm);

	// 功能:获取设备卡参数
	// 输入:deviceId	设备Id
	// 输入:slotIdx		槽位号
	// 输出:pCardParm	卡参数
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS GetCardParm(unsigned long deviceId, int slotIdx, CARD_PARM * pCardParm);

	// 功能:设置设备槽位卡通道参数
	// 输入:deviceId	设备Id
	// 输入:slotIdx		槽位号
	// 输入:ch			通道号(从0开始)
	// 输入:chParm		通道参数
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS SetCardChParm(unsigned long deviceId, int slotIdx, int ch, CH_PARM chParm );

	// 功能:获取设备槽位卡通道参数
	// 输入:deviceId	设备Id
	// 输入:slotIdx		槽位号
	// 输入:ch			通道号(从0开始)
	// 输出:pChParm		通道参数
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS GetCardChParm(unsigned long deviceId, int slotIdx, int ch, CH_PARM *pChParm );
	
	// 功能:设置设备(机箱)功能参数
	// 输入:deviceId	设备Id
	// 输入:devParam	设备(机箱)功能参数
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS SetDevFuncInfo(unsigned long deviceId, DEV_PARM devParam);
	// 功能:获取设备(机箱)功能参数
	// 输入:deviceId	设备Id
	// 输出:devParam	设备(机箱)功能参数
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS GetDevFuncInfo(unsigned long deviceId, DEV_PARM *devParam);

	// 功能:设置卡（功能模块）基本信息
	// 输入:deviceId	设备Id
	// 输入:slotIdx		槽位号
	// 输入:cardBaseinfo卡基本信息
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS SetCardBaseInfo(unsigned long deviceId, int slotIdx, CardBaseInfo cardBaseinfo);

	// 功能:读取卡（功能模块）基本信息
	// 输入:deviceId	设备Id
	// 输入:slotIdx		槽位号
	// 输出:pCardBaseinfo卡基本信息
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS GetCardBaseInfo(unsigned long deviceId, int slotIdx, CardBaseInfo *pCardBaseinfo);

	// 功能:读取调理模块功能信息
	// 输入:deviceId	设备Id
	// 输入:pScmfuncinfo->CardType	调理模块型号
	// 输出:pScmfuncinfo	(需传入pScmfuncinfo->CardType调理模块型号)调理模块功能信息
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS GetScmFuncInfo(unsigned long deviceId, SCM_FuncInfo *pScmfuncinfo);

	// 功能:设置设备基本信息
	// 输入:deviceId	设备Id
	// 输入:devinf		设备基本信息
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS SetDevBaseInfo(unsigned long deviceId, DevInf devinf);

	// 功能:读取设备的基本信息
	// 输入:deviceId	设备Id
	// 输出:pDevinf		设备基本信息
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS GetDevBaseInfo(unsigned long deviceId, DevInf* pDevinf);

	// 功能:读取卡（功能模块）功能信息
	// 输入:deviceId	设备Id
	// 输入:slotIdx		槽位号
	// 输出:pCardFuncInfo卡功能信息
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS GetCardFuncInfo(unsigned long deviceId, int slotIdx, CardFuncInfo *pCardFuncInfo);

	// 功能:设置设备IP(模块)
	// 输入:deviceId	设备Id
	// 输入:netIp		ip结构体
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS SetNetIp(unsigned long deviceId, NetIp netIp);
	
	// 功能:切换Server通过WIFI连接VIB
	// 输入:bWifi		是否切换至WIFI，1是，0不是
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS SetVibConnWifi( char bWifi);

	// 功能:读寄存器值（透传）
	// 输入:deviceId	设备Id
	// 输入:slotIdx		槽位号(默认填0)
	// 输入:regIdx		命令地址
	// 输出:regValue	寄存器值
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS GetTransparentValue(unsigned long deviceId, int slotIdx, unsigned long regIdx, unsigned long long &regValue);

	// 功能:写寄存器值（透传）
	// 输入:deviceId	设备Id
	// 输入:slotIdx		槽位号
	// 输入:regIdx		命令地址
	// 输入:regValue	寄存器值
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS SetTransparentValue(unsigned long deviceId, int slotIdx, unsigned long regIdx, unsigned long long regValue);


/***************************采集数据*************************/
	// 功能:组启动采集
	// 输入:deviceId	设备Id
	// 输入:groupid		组号(从0开始)
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS StartGroupAcq(unsigned long deviceId, int groupid);

	// 功能:组停止采集
	// 输入:deviceId	设备Id
	// 输入:groupid		组号(从0开始)
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS StopGroupAcq(unsigned long deviceId, int groupid);

	// 功能:组手动触发采集
	// 输入:deviceId	设备Id
	// 输入:groupid		组号(从0开始)
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS StartGroupTrg(unsigned long deviceId, int groupid);

	// 功能:获取设备组的采集状态
	// 输入:deviceId	设备Id
	// 输出:status		采集状态
	// 输入:groupid		组号(从0开始)
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS GetGroupStatus(unsigned long deviceId,int &status, int groupid);

	// 功能:获取数据头信息
	// 输入:deviceId	设备Id
	// 输入:slotIdx		槽位号
	// 输出:pDataHead	数据头信息
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS GetDataHead(unsigned long deviceId, int slotIdx, DataHeadINFO *pDataHead);

	// 功能:下载卡数据
	// 输入:deviceId	设备Id
	// 输入:slotIdx		槽位号
	// 输入:startdot	数据起点位置
	// 输入:dataLenth	下载数据长度
	// 输出:pData		数据
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS DownLoadCardData(unsigned long deviceId, int slotIdx, long long startdot, long long dataLenth, unsigned char *pData);
	
	// 功能:下载卡指定通道数据(仅支持机箱设备)
	// 输入:deviceId	设备Id
	// 输入:slotIdx		槽位号
	// 输入:ch			通道号
	// 输入:startdot	数据起点位置
	// 输入:dataLenth	下载数据长度
	// 输出:pData		数据
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS DownLoadCardChData(unsigned long deviceId, int slotIdx,int ch, long long startdot, long long dataLenth, unsigned char *pData);

	// 功能:下载卡实时数据（不带起点位置）
	// 输入:deviceId	设备Id
	// 输入:slotIdx		槽位号
	// 输入:dataLenth	下载数据长度(目前为固定长度64*1024个字节，不带数据头)
	// 输出:pData		数据
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS DownLoadRealData(unsigned long deviceId, int slotIdx, long long dataLenth, unsigned char *pData);

/***************************存储管理*************************/
	// 功能:设备储存容量
	// 输入:deviceId	设备Id
	// 输出:total		总容量(MB)
	// 输出:free		剩余容量(MB)
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS Capacity(unsigned long deviceId, double &total, double &free);
	// 功能:格式化设备储存空间
	// 输入:deviceId	设备Id
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS Format(unsigned long deviceId);
	// 功能:删除机箱设备上的文件
	// 输入:deviceId	设备Id
	// 输入:path		文件路径，字符串长度上限64
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS DelFile(unsigned long deviceId,char* path);
	// 功能:设置(模块)实时数据储存目录
	// 输入:dir 目录buff(长度固定128)
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS SetRtDataDir(char* dir);

	// 功能:获取机箱ftp文件路径
	// 输入:deviceId	设备Id
	// 输入:path		多个文件路径，字符串长度上限1024
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS GetFtpFilePath(unsigned long deviceId, char* path);

	// 功能:切割实时文件
	// 输入:deviceId	设备Id
	// 输入:groupid		组号
	// 输入:datetime	时间
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS CutRTFile(unsigned long deviceId, int groupid, RTfile_DateTime datetime);

	// 功能:切割实时文件进度
	// 输入:deviceId	设备Id
	// 输出:progress	进度
	// 返回:0正常 非0错误
	APIPVIB2_0_API PVIB_STATUS GetCutProgress(unsigned long deviceId, int &progress);
#ifdef __cplusplus
}
#endif

#endif
