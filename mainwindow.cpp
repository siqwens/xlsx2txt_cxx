#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->setRowCount(4);
    const QStringList name = {"Лицевой счет", "Номер кассы", "Номер счета", "Дата", "Сумма"};
    ui->tableWidget->setHorizontalHeaderLabels(name);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_openFile_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    if (dialog.exec()) {
        fullPath_ = dialog.selectedFiles()[0];
        fileName_ = dialog.selectedFiles()[0].split("/").last();
    }
    qDebug() << fileName_ << "|" << fullPath_;

    ui->groupBox->setTitle(fileName_);
    QXlsx::Document doc(fullPath_);
    int cols, rows;
    cols = doc.dimension().columnCount();
    rows = doc.dimension().rowCount();
    qDebug() << "ROWS --" << rows << " | "
             << "COLS --" << cols;
    ui->tableWidget->setRowCount(rows - 2); // <----- заменить на rows !!!!!!!
    QVariant var;
    if (doc.load()) {
        for (int i = 2; i < rows; ++i) {
            for (int cl = 1; cl < cols + 1; ++cl) {
                var = doc.read(i, cl);
                ui->tableWidget->setItem(i - 2, cl - 1, new QTableWidgetItem(var.toString()));
                //                qDebug() << "CELL read sucsses";
                //                qDebug() << "VAL at" << i << cl << "|" << var.toString();
            }
        }
    }
}

void MainWindow::on_btn_convert_clicked()
{
    QString str;
    QString fmt = "dd.MM.yyyy", dt;
    double sum;
    int rows = ui->tableWidget->rowCount();
    int cols = ui->tableWidget->columnCount();

    for (int rw = 0; rw < rows; ++rw) {
        for (int cl = 0; cl < cols; ++cl) {
            switch (cl) {
            case 0:
                str += ui->tableWidget->item(rw, cl)
                           ->data(Qt::DisplayRole)
                           .toString()
                           .rightJustified(22, ' ')
                       + "|";
                break;
            case 1:
                str += ui->tableWidget->item(rw, cl)
                           ->data(Qt::DisplayRole)
                           .toString()
                           .rightJustified(2, ' ')
                       + "|";
                break;
            case 2:
                if (ui->tableWidget->item(rw, 0)->data(Qt::DisplayRole).toString() == "") {
                    str += ui->tableWidget->item(rw, cl)
                               ->data(Qt::DisplayRole)
                               .toString()
                               .rightJustified(9, '0')
                           + "1|";
                } else {
                    QString val = ui->tableWidget->item(rw, 0)->data(Qt::DisplayRole).toString();
                    str += val.mid(2, 10) + "|";
                }

                break;
            case 3:
                dt = ui->tableWidget->item(rw, cl)->data(Qt::DisplayRole).toString();
                str += dt.mid(8, 2) + "." + dt.mid(5, 2) + "." + dt.mid(0, 4) + "|";
                // yyyy-mm-dd
                break;
            case 4:
                sum = ui->tableWidget->item(rw, cl)->data(Qt::DisplayRole).toDouble();
                str += QString::number(sum, 'f', 2).rightJustified(8, ' ') + "|";
                break;

            default:
                //                str += ui->tableWidget->item(rw, cl)->data(Qt::DisplayRole).toString() + "|";
                break;
            }
        }
        str += "43|78\n";
    }
    QString newFileName = fileName_.split(".")[0] + ".txt";
    QFile file(newFileName);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        stream.setEncoding(QStringConverter::Utf8);
        stream << str;
    }
    qDebug() << str;
}
