#ifndef COORDINATEPPOSIT_H
#define COORDINATEPPOSIT_H

#include <QWidget>

namespace Ui {
class CoordinatepPosit;
}

class CoordinatepPosit : public QWidget
{
    Q_OBJECT

public:
    explicit CoordinatepPosit(QWidget *parent = 0);
    ~CoordinatepPosit();


protected:
    void resizeEvent(QResizeEvent *event) override;
private:
    Ui::CoordinatepPosit *ui;






















    QString styleSheet = R"(
        /* GroupBox 样式 */
        QGroupBox {
            font-weight: bold;
            font-size: 12px;
            border: 2px solid #cccccc;
            border-radius: 8px;
            margin-top: 1ex;
            padding-top: 10px;
            background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                            stop: 0 #f6f7fa, stop: 1 #e5e7eb);
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top center;
            padding: 0 8px;
            background-color: #4a86e8;
            color: white;
            border-radius: 4px;
        }

        /* 按钮样式 */
        QPushButton {
            background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                            stop: 0 #6a9ef0, stop: 1 #4a86e8);
            border: 1px solid #3a76d8;
            border-radius: 6px;
            color: white;
            padding: 8px 16px;
            font-weight: bold;
        }

        QPushButton:hover {
            background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                            stop: 0 #7baef8, stop: 1 #5b96f0);
        }

        QPushButton:pressed {
            background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                            stop: 0 #3a76d8, stop: 1 #2a66c8);
        }

        /* 标签样式 */
        QLabel {
            font-weight: bold;
            color: #333333;
            padding: 4px;
        }

        /* 下拉框样式 */
        QComboBox {
            border: 1px solid #cccccc;
            border-radius: 4px;
            padding: 6px;
            background-color: white;
            selection-background-color: #4a86e8;
        }

        QComboBox:hover {
            border: 1px solid #999999;
        }

        /* 输入框样式 */
        QLineEdit, QSpinBox, QDoubleSpinBox {
            border: 1px solid #cccccc;
            border-radius: 4px;
            padding: 6px;
            background-color: white;
        }

        QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus {
            border: 2px solid #4a86e8;
        }

        /* 复选框样式 */
        QCheckBox {
            spacing: 8px;
            color: #333333;
        }

        QCheckBox::indicator {
            width: 16px;
            height: 16px;
            border: 1px solid #cccccc;
            border-radius: 3px;
            background-color: white;
        }

        QCheckBox::indicator:checked {
            background-color: #4a86e8;
            border: 1px solid #3a76d8;
        }
    )";
};

#endif // COORDINATEPPOSIT_H
