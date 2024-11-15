#ifndef CHILDWND_H
#define CHILDWND_H

#include <QTextEdit>
#include <QCloseEvent>
#include <QPrinter>

class ChildWnd : public QTextEdit
{
    Q_OBJECT
public:
    ChildWnd();
    
    enum ETextAlign
    {
        EAlignLeft = 1, EAlignCenter, EAlignRight, EAlignJustify, 
    };    
    enum ETextStyle
    {
        EListDisc = 1,      EListCircle,        EListSquare,        EListDecimal, 
        EListLowerAlpha,    EListUpperAlpha,    EListLowerRoman,    EListUpperRoman,
    };
    
    void newFile();
    bool loadFile(const QString &fileName);
    
    QString userFriendlyCurrentFile();      // 提取文件名
    QString currentFile() const { return _currFile; }   // 返回当前文件路径
    
    bool save();
    bool saveAs();
    bool saveFile(QString &fileName);
    
    void setCurrentFile(const QString &fileName);
    void mergeFormatOnWordOrSelection(const QTextCharFormat &);
    
    void setAlign(ETextAlign eAlign);
    void setStyle(int style);
    
signals:
    
protected:
    void closeEvent(QCloseEvent *) override;
    
    bool maybeSave();
    
private slots:
    void documentWasModified();
    
private:
    // 获取较短的绝对路径
    QString strippedName(const QString &fullFileName);
    
    QString     _currFile;          // 当前文件路径
    bool        _isUntitled;        // 当前文件是否被保存到硬盘上
};

#endif // CHILDWND_H
