#include "childwnd.h"
#include <QFileInfo>
#include <QTextCodec>
#include <QFileDialog>
#include <QTextList>
#include <QMessageBox>
#include <QTextDocumentWriter>

ChildWnd::ChildWnd()
{
    setAttribute(Qt::WA_DeleteOnClose);
    _isUntitled = true;
}

void ChildWnd::newFile()
{
    static int sequenceNumber = 1;
    _isUntitled = true;
    _currFile = tr("文档%1").arg(sequenceNumber++);
    
    setWindowTitle(_currFile + "[*]" + tr(" - Myself Word"));
    connect(document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));
}

bool ChildWnd::loadFile(const QString &fileName)
{
    bool bRet = false;
    if (!fileName.isEmpty() && QFile::exists(fileName))
    {
        QFile file(fileName);
        if (file.open(QFile::ReadOnly))
        {
            const QByteArray &data = file.readAll();
            QTextCodec *codec = Qt::codecForHtml(data);
            QString str = codec->toUnicode(data);
            
            if (Qt::mightBeRichText(str))
            {
                this->setHtml(str);
            }
            else
            {
                str = QString::fromLocal8Bit(data);
                this->setPlainText(str);
            }
            
            // 设置当前文件
            setCurrentFile(fileName);
            connect(document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));
            
            bRet = true;
        }
        else
        {            
        }
    }
    else
    {
    }
    
    return bRet;
}

QString ChildWnd::userFriendlyCurrentFile()
{
    return strippedName(_currFile);
}

void ChildWnd::closeEvent(QCloseEvent *event)
{
    if (maybeSave())
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void ChildWnd::setCurrentFile(const QString &fileName)
{
    // canonicalFilePath可除去路径中的符号连接,如"."和".."等
    _currFile = QFileInfo(fileName).canonicalFilePath();
    _isUntitled = false;    // 文件已经被保存过
    document()->setModified(false);
    setWindowModified(false);   // 窗口不显示被更改标志
    
    // 设置窗口标题,userFriendlyCurrentFile()返回文件名
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

bool ChildWnd::maybeSave()
{
    if (!document()->isModified())
        return true;
    QMessageBox::StandardButton btn;
    btn = QMessageBox::warning(this, tr("Myself Qt Word"), tr("文档%1已被修改,是否保存?").arg(userFriendlyCurrentFile()), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    
    if (btn == QMessageBox::Save)
    {
        return save();
    }
    else if (btn == QMessageBox::Cancel)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void ChildWnd::documentWasModified()
{
    setWindowModified(document()->isModified());
}

QString ChildWnd::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

bool ChildWnd::save()
{
    if (_isUntitled)
    {
        return saveAs();
    }
    else
    {
        return saveFile(_currFile);
    }
}

bool ChildWnd::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("另存为"), _currFile, tr("HTML 文档 (*.htm *.html);;所有文件 (*.*)"));
    if (fileName.isEmpty())
    {
        return false;
    }
    else
    {
        return saveFile(fileName);
    }
}

bool ChildWnd::saveFile(QString &fileName)
{
    if (!(fileName.endsWith(".htm", Qt::CaseInsensitive) || fileName.endsWith(".html", Qt::CaseInsensitive)))
    {
        fileName += ".html";        // 默认保存为HTML文档
    }
    
    QTextDocumentWriter wr(fileName);
    bool bRet = wr.write(this->document());
    if (bRet)
    {
        setCurrentFile(fileName);
    }
    
    return bRet;
}

void ChildWnd::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = this->textCursor();
    //if (!cursor.hasSelection())
    //{
    //    cursor.select(QTextCursor::WordUnderCursor);
    //}
    cursor.mergeCharFormat(format);
    this->mergeCurrentCharFormat(format);
}

void ChildWnd::setAlign(ETextAlign eAlign)
{
    switch (eAlign)
    {
    case EAlignLeft:
        this->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
        break;
    case EAlignCenter:
        this->setAlignment(Qt::AlignHCenter);
        break;
    case EAlignRight:
        this->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
        break;
    case EAlignJustify:
        this->setAlignment(Qt::AlignJustify);
        break;
    default:
        break;
    }
}
void ChildWnd::setStyle(int style)
{
    QTextCursor cursor = this->textCursor();
    if (0 != style)
    {
        QTextListFormat::Style styleName = QTextListFormat::ListDisc;
        
        switch (style)
        {
        case EListDisc:
            styleName = QTextListFormat::ListDisc;      //实心圆标号
            break;
        case EListCircle:
            styleName = QTextListFormat::ListCircle;    //空心圆标号
            break;
        case EListSquare:
            styleName = QTextListFormat::ListSquare;    //方形标号
            break;
        case EListDecimal:
            styleName = QTextListFormat::ListDecimal;   //十进制编号
            break;
        case EListLowerAlpha:
            styleName = QTextListFormat::ListLowerAlpha; //小写字母编号
            break;
        case EListUpperAlpha:
            styleName = QTextListFormat::ListUpperAlpha; //大写字母编号
            break;
        case EListLowerRoman:
            styleName = QTextListFormat::ListLowerRoman; //小写罗马编号
            break;
        case EListUpperRoman:
            styleName = QTextListFormat::ListUpperRoman; //大写罗马编号
            break;
        default:
            break;
        }
        
        // 为支持撤销操作
        cursor.beginEditBlock();
        QTextBlockFormat blockFmt = cursor.blockFormat();
        QTextListFormat listFmt;
        
        if (cursor.currentList())
        {
            listFmt = cursor.currentList()->format();
        }
        else
        {
            listFmt.setIndent(blockFmt.indent() + 1);
            blockFmt.setIndent(0);
            cursor.setBlockFormat(blockFmt);
        }
        
        listFmt.setStyle(styleName);
        cursor.createList(listFmt);
        cursor.endEditBlock();
    }
    else
    {
        QTextBlockFormat bfmt;
        bfmt.setObjectIndex(-1);
        cursor.mergeBlockFormat(bfmt);
    }
}
