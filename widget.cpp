#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
{
    QTextStream cout(stdout, QIODevice::WriteOnly);

    //放置原始文本
    QVector<QString> Prev_String;
    //放置格式化后结果
    QVector<QString> Cur_String;


    //用于控制缩进
    static QChar indent = ' ';

    //一些符号（暂未使用）
    QSet<QChar> calc = {'+','-','*','/','=','%','^'};
    QSet<QChar> comp = {'&','|','<','>','!'};
    QSet<QChar> spec = {'#','?',',','.',':','~',';'};

    //分别放置不同类别的关键字
    QVector<QString> Need_Back = {"IF","CASE","FOR","WHILE","REPEAT","VAR"};
    QVector<QString> Need_Front = {"END_IF","END_CASE","END_FOR","END_REPEAT","END_VAR"};
    QVector<QString> NO_Need = {"ELSIF","ELSE","UNTIL"};
    //格式化后结果的QString状态，用于输出
    QString Final_String;
    //每一行做处理的工作QString
    QString Work_String;


    //需要缩进的次数
    int Count_indent = 0;

    //是否捕捉到了关键字
    bool catch_keyword = false;
    //是否捕捉到了CASE
    bool catch_CASE = false;
    //是否是注释模式
    bool is_note = false;


    QTextDocument* doc = ui->textEdit->document();
    //文本行数
    int count = doc->blockCount();

    //按行读取到Prev_String中
    for(int i = 0;i < count;i++){
        QTextBlock textline = doc->findBlockByNumber(i);
        QString str = textline.text();
        Prev_String.push_back(str);
    }

    //格式化Prev_String，并写入Cur_String
    for(QString str : Prev_String){
        //工作QString初始化
        Work_String = "";
        int str_size = str.size();
        catch_keyword = false;
        //逐个字符进行处理
        for(int idx = 0;idx < str_size;idx++){
            if(!is_note){
                //当前不是注释模式

                //开头缩进控制以及关键字匹配
                if(idx == 0){
                    while(str[idx] == ' ' || str[idx] == '\t')
                        idx++;
                    //CASE关键字特殊判断
                    if(catch_CASE){
                        if(str.mid(idx,4) == "ELSE"){
                            QChar ch = str[idx+4];
                            if(ch == ' ' || ch == ';' || idx+4 == str_size || ch == '\t'){
                                catch_CASE = false;
                            }
                        }else if(str.mid(idx,8) == "END_CASE"){
                            QChar ch = str[idx+8];
                            if(ch == ' ' || ch == ';' || idx+8 == str_size || ch == '\t'){
                                catch_CASE = false;
                            }
                        }else{
                            while(str[idx] != ':' && idx < str_size){
                                Work_String += str[idx++];
                            }
                            idx++;
                            Work_String += ':';
                        }

                    }
                    //控制缩进部分
                    QString fill_test = "";
                    Work_String += fill_test.fill(indent,Count_indent * 4);


                    if(catch_CASE)
                        Work_String.chop(1);

                }


                //判断需要缩进的关键字
                if(!catch_keyword){
                    for(QString NB : Need_Back){
                        int length_str = NB.size();
                        if(str.mid(idx,length_str) == NB){
                            QChar ch = str[idx + length_str];
                            if(ch == ' ' || ch == ';' || idx + length_str == str_size || ch == '\t'){
                                if(NB == "CASE")
                                    catch_CASE = true;
                                Work_String += NB;
                                idx += length_str;
                                Count_indent++;
                                catch_keyword = true;
                            }
                        }
                    }
                }

                //判断需要跳出的关键字
                if(!catch_keyword){
                    for(QString NF : Need_Front){
                        int length_str = NF.size();
                        if(str.mid(idx,length_str) == NF){
                            QChar ch = str[idx + length_str];
                            if(ch == ' ' || ch == ';' || idx + length_str == str_size || ch == '\t'){
                                Work_String.chop(4);
                                Work_String += NF;
                                idx += length_str;
                                Count_indent--;
                                catch_keyword = true;
                            }
                        }
                    }
                }

                //判断需要回退的关键字
                if(!catch_keyword){
                    for(QString NN : NO_Need){
                        int length_str = NN.size();
                        if(str.mid(idx,length_str) == NN){
                            QChar ch = str[idx + length_str];
                            if(ch == ' ' || ch == ';' || idx + length_str == str_size || ch == '\t'){
                                Work_String.chop(4);
                                Work_String += NN;
                                idx += length_str;
                                catch_keyword = true;
                            }
                        }
                    }
                }



                //检测到空格则优化至一个空格
                if(str[idx] == ' ' || str[idx] == '\t'){
                    while(str[idx] == ' ' || str[idx] == '\t')
                        idx++;
                    Work_String += ' ';
                }

                if(str[idx] == '(' && str[idx+1] == '*'){
                    //进入注释状态
                    Work_String += "(*";
                    idx += 2;
                    is_note = !is_note;
                }



                catch_keyword = true;
            }else{
                //当前处于注释模式
                if(str[idx] == '*' && str[idx+1] == ')'){
                    Work_String += "*)";
                    idx += 2;
                    is_note = !is_note;
                }

                //检测到空格则优化至一个空格
                if(str[idx] == ' ' || str[idx] == '\t'){
                    while(str[idx] == ' ' || str[idx] == '\t')
                        idx++;
                    Work_String += ' ';
                }

            }


            //遇到计算符号的处理
            if(calc.find(str[idx]) != calc.end()){
                cout << "find one calc" << endl;

            }


            if(idx < str_size)
                Work_String += str[idx];
        }

        //将修改后的当前行加入到vector中
        if(!Work_String.isEmpty())
           Cur_String.push_back(Work_String);
    }


    for(QString str : Cur_String){
        Final_String += str + '\n';

    }


    ui->textEdit->setText(Final_String);
    return ;
}
