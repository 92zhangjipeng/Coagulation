#ifndef STRUCTQUALITYCONTROL_H
#define STRUCTQUALITYCONTROL_H

#pragma once
#include <QObject>
#include <QGlobalStatic>

class structQualityControl : public QObject {
	Q_OBJECT
	Q_DISABLE_COPY(structQualityControl)  // 禁用拷贝操作 

public:
	// 获取单例实例（线程安全）
	static structQualityControl& instance() {
		static structQualityControl singleton;  // 局部静态变量保证初始化线程安全
		return singleton;
	}

	// 示例：定义Qt属性（Q_PROPERTY）
	Q_PROPERTY(int inspectionLevel READ inspectionLevel WRITE setInspectionLevel NOTIFY inspectionLevelChanged)

	// 属性访问方法
	int inspectionLevel() const { return m_inspectionLevel; }
	void setInspectionLevel(int level) {
		if (m_inspectionLevel != level) {
			m_inspectionLevel = level;
			emit inspectionLevelChanged(level);
		}
	}



signals:
	void inspectionLevelChanged(int newLevel);  // 属性变更信号 

public slots:
	void performQualityCheck() {
		/* 质量检测逻辑 */
	}

private:
	// 私有构造函数（初始化QObject父类）
	explicit structQualityControl(QObject *parent = nullptr)
		: QObject(parent), m_inspectionLevel(1) {}

	// 成员变量
	int m_inspectionLevel;


};

#endif // STRUCTQUALITYCONTROL_H




//QualityControl ii = QualityControl::instance();
