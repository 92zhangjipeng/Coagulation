#ifndef CUSTOMPLOT_H
#define CUSTOMPLOT_H

#include <QButtonGroup>
#include <QTableWidget>
#include <QWidget>
#include <QScrollBar>
#include <QSpinBox>
#include <QPair>
#include <QPointer>
#include "testing.h"
#include <QStyledItemDelegate>
#include "CameraWindow.h"
#include <memory>  // 添加智能指针支持

namespace Ui {
	class CustomPlot;
}

// 配置命名空间
namespace CustomPlotConfig {
	constexpr int kMaxBloodZone = 12;
	constexpr int kMaxTubeTray = 4;
	constexpr quint8 kTotalTubeNum = 60;
	constexpr int kDefaultComboBoxRowHeight = 30;

	const QString kButtonStyle = QStringLiteral(
		"QToolButton{border-image: url(:/Picture/SetPng/btninit.png);font-family:'楷体';}"
		"QToolButton:checked{border-image: url(:Picture/SetPng/btnpushing.png);font-family:'楷体';}"
		"QToolButton:pressed{border-image: url(:/Picture/SetPng/btnpush.png);font-family:'楷体';}"
		"QToolButton:hover{border-image: url(:/Picture/SetPng/btnon.png);font-family:'楷体';}"
		);
}



class MyQStyledItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	MyQStyledItemDelegate(int height, QObject *parent) : QStyledItemDelegate(parent), m_Height(height) {}
	~MyQStyledItemDelegate() {}

public:
	int m_Height;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
		QSize size = QStyledItemDelegate::sizeHint(option, index);
		size.setHeight(m_Height);    //这里设置高度
		return size;
	}
	void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const {
		QStyledItemDelegate::paint(painter, option, index);
	}
};

/**
* @enum InstrumentCoordinate
* @brief 仪器坐标枚举定义
*
* 定义样本针、试剂针、抓手等仪器的坐标位置，用于仪器校准和位置调整
*/
enum InstrumentCoordinate {
    Origin_bloodNeedle = 0,        ///< 样本针原点坐标
    CleanLinque_bloodNeedle = 1,   ///< 样本针清洗区坐标
    Bloodsample_BloodNeedle = 2,   ///< 样本针样本区坐标
    EmptyTube_1_BloodNeedle = 3,   ///< 样本针试杯区1坐标
    EmptyTube_2_BloodNeedle = 4,   ///< 样本针试杯区2坐标
    EmptyTube_3_BloodNeedle = 5,   ///< 样本针试杯区3坐标
    EmptyTube_4_BloodNeedle = 6,   ///< 样本针试杯区4坐标
	CleanLinque_ReagentNeedle = 7, ///< 试剂针清洗区坐标
	ReagentLinque_ReagentNeedle = 8, ///< 试剂针试剂区坐标
	TestChannel_ReagentNeedle = 9, ///< 试剂针测试区坐标
	TestChannel_Hands = 10,         ///< 抓手测试区坐标
	ThrowCup_Hands = 11,           ///< 抓手弃杯孔坐标
	EmptyTube_1_Hands = 12,         ///< 抓手试杯区1坐标
	EmptyTube_2_Hands = 13,         ///< 抓手试杯区2坐标
	EmptyTube_3_Hands = 14,         ///< 抓手试杯区3坐标
	EmptyTube_4_Hands = 15,         ///< 抓手试杯区4坐标
};

/**
* @enum NotifyHolePos
* @brief 空试管孔位置枚举定义
*
* 定义空试管托盘四个角的孔位置编号，用于坐标校准和位置识别
*/
enum NotifyHolePos {
	Empty_1_topLeft = 13,       ///< 空试管托盘1左上角孔号
	Empty_1_topRight = 16,      ///< 空试管托盘1右上角孔号
	Empty_1_bottomLeft = 43,    ///< 空试管托盘1左下角孔号
	Empty_1_bottomRight = 46,   ///< 空试管托盘1右下角孔号
	Empty_2_topLeft = 73,       ///< 空试管托盘2左上角孔号
	Empty_2_topRight = 76,      ///< 空试管托盘2右上角孔号
	Empty_2_bottomLeft = 103,   ///< 空试管托盘2左下角孔号
	Empty_2_bottomRight = 106,  ///< 空试管托盘2右下角孔号
	Empty_3_topLeft = 133,      ///< 空试管托盘3左上角孔号
	Empty_3_topRight = 136,     ///< 空试管托盘3右上角孔号
	Empty_3_bottomLeft = 163,   ///< 空试管托盘3左下角孔号
	Empty_3_bottomRight = 166,  ///< 空试管托盘3右下角孔号
	Empty_4_topLeft = 193,      ///< 空试管托盘4左上角孔号
	Empty_4_topRight = 196,     ///< 空试管托盘4右上角孔号
	Empty_4_bottomLeft = 223,   ///< 空试管托盘4左下角孔号
	Empty_4_bottomRight = 226,  ///< 空试管托盘4右下角孔号
};

/**
* @enum AreasCalibration
* @brief 校准区域枚举定义
*
* 定义仪器中需要校准的不同区域，用于坐标校准和位置调整
*/
enum AreasCalibration {
	origin = 0,         ///< 原点区域
	detergent,          ///< 清洗液区域
	Reagentarea,        ///< 试剂区域
	testchannel,        ///< 测试通道区域
	Discardhole,        ///< 弃杯孔区域
	Testtubearea1,      ///< 试管区域1
	Testtubearea2,      ///< 试管区域2
	Testtubearea3,      ///< 试管区域3
	Testtubearea4,      ///< 试管区域4
	bloodsamplearea     ///< 血样区域
};
/**
* @enum IndexZ
* @brief Z轴索引枚举定义
*
* 定义不同仪器在Z轴方向的位置索引
*/
enum IndexZ {
	Hands_z = 0,    ///< 抓手Z轴位置
	Reagent_z,      ///< 试剂针Z轴位置
    Blood_z         ///< 样本针Z轴位置
};

/**
* @enum TheGripperDrops
* @brief 抓手下降高度枚举定义
*
* 定义抓手下降的不同高度级别
*/
enum TheGripperDrops {
	GipperDown_top = 0,      ///< 抓手下降高度：顶部
	GipperDown_mid,          ///< 抓手下降高度：中部
	GipperDown_bottom,       ///< 抓手下降高度：底部
	GipperDown_custom        ///< 抓手下降高度：自定义
};

/**
* @enum TableIndexPos
* @brief 表格索引位置枚举定义
*
* 定义坐标表格中各列的索引位置
*/
enum TableIndexPos {
	IndexNum = 0,           ///< 索引号列
	Instrument_xpos,        ///< 仪器X坐标列
	Instrument_ypos,        ///< 仪器Y坐标列
	Instrument_save         ///< 仪器保存状态列
};

/**
* @class CustomPlot
* @brief 自定义坐标调整界面类
*
* 提供仪器坐标微调、校准区域设置、试管移动测试等功能的主界面类
* 支持样本针、试剂针、抓手等仪器的坐标校准和位置调整
*/
class CustomPlot : public QWidget
{
	Q_OBJECT

public:
	/**
	* @brief 构造函数
	* @param parent 父窗口指针
	*/
	explicit CustomPlot(QWidget *parent = 0);

	/**
	* @brief 析构函数
	*/
	~CustomPlot();

	/**
	* @brief 初始化界面样式
	* @param equipmentType 设备类型
	*/
	void initstyle(const quint8 equipmentType);
protected:
	//这是一个虚函数，继承自QEvent.只要重写了这个虚函数，当你按下窗口右上角的"×"时，就会调用你所重写的此函数.
	void closeEvent(QCloseEvent*event);
	private slots:

	/** 抓手下降高度
	* @brief CheckGipperDownHigh
	*/
	void  CheckGipperDownHigh(QAbstractButton*);

	/** 保存/复原 修改坐标
	* @brief BtnClickSavePoint
	*/
	void  BtnClickSavePoint();


	/** 基础改变其它也改变/复原
	* @brief BtnClickOthersSavePoint
	*/
	void  BtnClickOthersSavePoint();

	/**修改基础值
	* @brief UpdateBaseValue_x
	*/
	void  UpdateBaseValue_x(int, QSpinBox *pbox);
	void  UpdateBaseValue_y(int);

	/** 表格添加按钮
	* @brief AddBtnSave_backValue
	*/
	void AddBtnSave_backValue(int rowIndex, bool NeedChaneOther);
	void on_comboBox_calibrationAarea_activated(int index);

	void on_toolButton_Backorigin_clicked();

	void on_spinBox_downValue_valueChanged(int arg1);
	void on_pushButton_TrayHands_2_clicked();
	void on_toolButton_video_clicked();

private:
	/**
	* @brief 初始化显示坐标表格
	* @param Tablewidgetpos 坐标表格控件指针
	*/
	void InitdisplayPointTablewidget(QTableWidget* Tablewidgetpos);


    void sendCommder(const int indexAct, const QByteArrayList cmd, QString info, QPoint locAAxis);

	/**
	* @brief 初始化血样区域孔号
	* @param indexModels 设备型号
	*/
	void initBloodZoneNum(quint8 indexModels);

	/**
	* @brief 初始化下拉框视图
	* @param equipmentType 设备类型
	*/
	void initCommboxView(quint8 equipmentType);

	/**
	* @brief 设置项目背景颜色
	*/
	void setItemBackgroundColors();

	/**
	* @brief 隐藏列表视图中的指定行
	* @param view 列表视图指针
	* @param rows 需要隐藏的行索引列表
	*/
	void hideRows(QListView* view, const std::vector<int>& rows);

	/**
	* @brief 根据设备类型隐藏特定行
	* @param equipmentType 设备类型
	*/
	void hideRowsByEquipmentType(quint8 equipmentType);

	/**
	* @brief 初始化空试管孔号
	* @param indexType 设备类型
	*/
	void initEmptyTubeHole(quint8 indexType);

	/**
	* @brief 初始化测试通道孔号
	* @param indexType 设备类型
	*/
	void initTestChnHole(quint8 indexType);

	/**
	* @brief 用户点击原点校验位置
	*/
	void userClickOriginAxis();

    void uesrClickThrowHole();

    void userClickCleanHole();

	/**
	* @brief 初始化SpinBox样式
	*/
	void initSpinBoxStyle();

	/**
	* @brief 创建标准的SpinBox控件
	* @param value 初始值
	* @param style 样式字符串
	* @param parent 父控件
	* @return 创建的SpinBox控件指针
	*/
	QSpinBox* createStandardSpinBox(int value, const QString& style, QWidget* parent = nullptr);

	/**
	* @brief 创建居中的SpinBox控件
	* @param value 初始值
	* @param style 样式字符串
	* @return 创建的SpinBox控件指针
	*/
	QWidget* createCenteredSpinBox(int value, const QString& style);

	/**
	* @brief 创建居中的控件容器
	* @param contentWidget 内容控件
	* @return 创建的容器控件指针
	*/
	QWidget* createCenteredWidget(QWidget* contentWidget);

	/**
	* @brief 创建居中的按钮控件
	* @param text 按钮文本
	* @param rowIndex 行索引
	* @param needChangeOther 是否需要改变其他控件
	* @return 创建的按钮控件指针
	*/
	QWidget* createCenteredButton(const QString& text, int rowIndex, bool needChangeOther);

	/**
	* @brief 配置工具按钮
	* @param button 按钮指针
	* @param text 按钮文本
	* @param clickHandler 点击事件处理函数
	*/
	void configureToolButton(QToolButton* button, const QString& text,
		const std::function<void()>& clickHandler);

	/**
	* @brief 配置按钮组
	* @param button 按钮指针
	* @param group 按钮组指针
	* @param clickHandler 点击事件处理函数
	*/
	void configureButtonGroup(QToolButton* button, QButtonGroup* group,
		const std::function<void()>& clickHandler);

	/**
	* @brief 获取血样区域显示数量
	* @param model 设备型号
	* @return 血样区域数量
	*/
	int getBloodZoneShowCount(quint8 model) const;

	/**
	* @brief 获取试管托盘显示数量
	* @param model 设备型号
	* @return 试管托盘数量
	*/
	int getTubeTrayShowCount(quint8 model) const;

	/**
	* @brief 机器复位到原点
	*/
	void backMachinOrigin();

	/**
	* @brief 显示/修改原点坐标
	* @param bFindShow true显示坐标，false修改坐标
	* @param bWrite_x true修改X坐标，false修改Y坐标
	* @param notifyValue 修改的值
	*/
	void displayOriginAxisPoint(bool bFindShow, bool bWrite_x, int notifyValue);

	/**
	* @brief 显示/修改弃杯孔坐标
	* @param bFindShow true显示坐标，false修改坐标
	* @param bWrite_x true修改X坐标，false修改Y坐标
	* @param notifyValue 修改的值
	*/
	void displayThrowHole(bool bFindShow, bool bWrite_x, int notifyValue);

	/**
	* @brief 显示/修改清洗区坐标
	* @param bFindShow true显示坐标，false修改坐标
	* @param indexZ Z轴索引
	* @param bWrite_x true修改X坐标，false修改Y坐标
	* @param notifyValue 修改的值
	*/
	void displayCleanLinqueAxisPoint(bool bFindShow, quint8  indexZ, bool bWrite_x, int notifyValue);

	/**
	* @brief 显示/修改试剂区坐标
	* @param bFindShow true显示坐标，false修改坐标
	* @param bWrite_x true修改X坐标，false修改Y坐标
	* @param notifyValue 修改的值
	* @return 操作结果状态码
	*/
	int displayReagentAxisPoint(bool bFindShow, bool bWrite_x, int notifyValue);

	/**
	* @brief 显示/修改测试通道坐标
	* @param bFindShow true显示坐标，false修改坐标
	* @param bWrite_x true修改X坐标，false修改Y坐标
	* @param indexZ Z轴索引
	* @param ChnNum 通道编号
	* @param notifyValue 修改的值
	*/
	void displayTestChnAxisPoint(bool bFindShow, bool bWrite_x, quint8 indexZ, quint8 ChnNum, int notifyValue);

	/**
	* @brief 显示/修改空试管托盘坐标
	* @param bFindShow true显示坐标，false修改坐标
	* @param bWrite_x true修改X坐标，false修改Y坐标
	* @param indexZ Z轴索引
	* @param indexTray 托盘索引
	* @param tubeNum 试管编号
	* @param notifyValue 修改的值
	* @return 操作结果状态码
	*/
	int displayEmptyTrayAxisPoint(bool bFindShow, bool bWrite_x, quint8 indexZ,
		quint8 indexTray, quint8 tubeNum, int notifyValue);

	/**
	* @brief 显示单个坐标点的通用模板
	* @param bFindShow true显示坐标，false修改坐标
	* @param getPointFunc 获取坐标点的函数
	* @param description 描述文本
	*/
	template<typename Func>
	void displaySinglePointAxis(bool bFindShow, Func getPointFunc, const QString& description)
	{
		if (bFindShow) {
			QPoint point;
			getPointFunc(point);
			QMap<quint8, QPoint> points = { { 0, point } };
			notifyShowInstrumentPoint(points, description);
		}
	}

	/**
	* @brief 显示/修改血样区坐标
	* @param bFindShow true显示坐标，false修改坐标
	* @param bWrite_x true修改X坐标，false修改Y坐标
	* @param indexhole 孔编号
	* @param notifyValue 修改的值
	* @return 操作结果状态码
	*/
	int displayBloodHoleAxisPoint(bool bFindShow, bool bWrite_x, quint8 indexhole, int notifyValue);

	/**
	* @brief 显示仪器区域坐标
	* @param displayPoint 坐标点集合
	* @param tableName 表格名称
	*/
	void notifyShowInstrumentPoint(QMap<quint8, QPoint> &displayPoint, const QString &tableName);

	/**
	* @brief 修改一个坐标，其他坐标跟随变化
	* @param displayPoint 坐标点集合
	* @param tableName 表格名称
	*/
	void InsertOneChangeOthersChange(QMap<quint8, QPoint> displayPoint, const QString tableName);

	// 设置序号列（不可编辑、居中、灰色背景）
	void setupIndexColumn(int row);
	// 设置表格行
	void setupTableRow(int row, const QPoint& point, bool isEditable);


	/**
	* @brief 更新列表的偏移值
	* @param bupdate_x true更新X偏移，false更新Y偏移
	* @param offsetValue 偏移值
	*/
	void updateNotifyOffsetValue(bool bupdate_x, const int offsetValue);



	/**
	* @brief 刷新所有行的显示（同步数据）
	*/
	void refreshAllRowsDisplay();

	/**
	* @brief 写入控制板指令
	* @param intdexZ Z轴索引
	* @param row 行号
	* @param sendcommd 发送指令列表
	*/
	void writeboard(quint8 intdexZ, quint8 row, QByteArrayList &sendcommd);

	/**
	* @brief 查询并修改坐标
	* @param PointGather 坐标集合
	* @param FindIndexKey 查找的索引键
	* @param FindedCoordinates 找到的坐标
	* @return 查找是否成功
	*/
	bool QuerytoModifyCoordinates(QMap<int, QPoint> PointGather, int FindIndexKey, QPoint &FindedCoordinates);

	/**
	* @brief 选中行改变背景色
	* @param changeColor 是否改变颜色
	* @param selectedIndex 选中的索引
	* @param indexTray 托盘索引
	*/
	void SelectHoleChangebgm(bool changeColor, int selectedIndex, int indexTray);

	/**
	* @brief 修改控件背景色并还原
	* @param bChangecolor 是否改变颜色
	* @param selectedIndex 选中的索引
	*/
	void ChangeControlColors(bool bChangecolor, int selectedIndex);

	/**
	* @brief 清空表格文本
	*/
	void DelTableText();

	/**
	* @brief 显示测试区试管坐标抓手
	* @param TableRow 表格行号
	*/
	void ReminderTable(int TableRow);

	/**
	* @brief 测试抓取试管杯命令
	* @param form_ 起始位置
	* @param end_ 目标位置
	* @param group_directives 指令组
	*/
	void testCatchPutDownCup(const int form, const int end, QMap<int, QByteArrayList> &groupDirectives);
    void continueCatchCups();
	/**
	* @brief 移动测试命令发送到机器
	* @param indexZone 区域索引
	* @param MovingAxis 移动坐标
	* @param indexZ Z轴索引
	*/
	void _sendmoveActiveToEquipment(AreasCalibration indexZone, QPoint MovingAxis, quint8 indexZ);

	/**
	* @brief 退出调试时写入缓存坐标
	*/
	void ExitCoordinateSaving();

    //测试抓手连续抓取
    QByteArrayList takeFirstFromMap(QMap<int, QByteArrayList>& map);
    bool sednCatchCups(QMap<int, QByteArrayList>& catchMap);


signals:
	/**
	* @brief 写入轴数据信号
	* @param data 轴数据列表
	* @param description 数据描述
	*/
	void writdAxisata(const QByteArrayList data, QString description);

	/**
	* @brief 仪器校准动作信号
	* @param ACtionType 动作类型
	* @param GroupAction 动作组
	*/
    void testMachineAxis(const int ACtionType, const QByteArrayList GroupAction);
	/**
	* @brief 试管托盘运动测试信号
	* @param commands 指令列表
	* @param isAccurate 是否精确模式
	* @param count 指令数量
	*/
	void TrayMoveTest(const QByteArrayList commands, const bool isAccurate, const quint8 count);

	public slots:
	/**
	* @brief 点击血样试管槽函数
	*/
	void ClickBloodTube();

	/**
	* @brief 点击空试管槽函数
	*/
	void ClickEmptyTube();

	/**
	* @brief 点击测试通道试管槽函数
	*/
	void ClickTestChannelTube();

	/**
	* @brief 点击试剂试管槽函数
	*/
	void ClickReagentsTube();

	/**
	* @brief 移动测试高度槽函数
	* @param x X坐标
	* @param y Y坐标
	* @param downMm 下降毫米数
	*/
	void onMoveTestHeigh(const int x, const int y, const double downMm);

	/**
	* @brief 返回原点槽函数
	*/
	void onbackOrigin();

	/**
	* @brief 测试吸取动作槽函数
	* @param data 动作数据
	*/
	void onTestSuckPrpAct(const QByteArrayList data);

	/**
	* @brief 收到校准消息完成槽函数
	*/
    void recvCalibrationMoved(const int finishAct);





	/**
	* @brief 发送代码槽函数
	* @param data_ 数据
	*/
	void slotsendcode(QByteArrayList data_);

private:
	std::unique_ptr<Ui::CustomPlot> ui;  // 使用智能指针管理UI
	QFont mfont;

	QButtonGroup* m_CheckGroupBox = nullptr;

	int m_downhigh;  //抓手下降高度
	int m_indexDown = -1;
	int m_CalibrationArea;  //选取的区域
	QPoint mbtnClickPos;   //单击的坐标
	quint8   mcodeNum;       //命令编号
	QFont mcustFont;

	QPointer<CameraWindow> mCameraWindow;


	QMap<int, QPoint> mReagentLinqueVaue;                    ///< 记录初始列表内的值 - 试剂液、试剂针
	QMap<int, QPoint> mEmptytestcup_offset_Hands[4];         ///< 空测试杯、抓手偏移坐标
    QMap<int, QPoint> mEmptytestcup_offset_BloodNeedle[4];   ///< 空测试杯、样本针偏移坐标
	QMap<int, QPoint> mSampleBloodPos;                       ///< 血样区坐标
	QButtonGroup * mbloodBtnGroupbox = nullptr;             ///< 血样按钮组
	quint8  mInstrumentType;                                ///< 仪器类型
	int     m_selectRows = -1;                              ///< 选中行索引

	const  QString m_Cssbtn = QStringLiteral(
		"QToolButton{border-image: url(:/Picture/SetPng/btninit.png);font-family:'楷体';}"
		"QToolButton:checked{border-image: url(:Picture/SetPng/btnpushing.png);font-family:'楷体';}"
		"QToolButton:pressed{border-image: url(:/Picture/SetPng/btnpush.png);font-family:'楷体';}"
		"QToolButton:hover{border-image: url(:/Picture/SetPng/btnon.png);font-family:'楷体';}"
		);                                                      ///< 按钮样式表

	QString m_SpinboxsheetX;                                ///< X轴SpinBox样式表
	QString m_SpinboxsheetY;                                ///< Y轴SpinBox样式表

	QMap<int, QByteArrayList> mtest_catch_put_commad;        ///< 测试抓取放置命令
	QVector<int> mfromHole;                                 ///< 起始孔位列表
	QVector<int> mendHole;                                  ///< 目标孔位列表
	int mfromtray;                                          ///< 起始托盘索引
	int mendtray;                                           ///< 目标托盘索引
	bool mbstopTestadjust;                                  ///< 测试调整停止标志
	bool m_testInProgress = false;                          ///< 测试进行中标志
};

#endif // CUSTOMPLOT_H
