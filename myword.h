#ifndef MYWORD_H
#define MYWORD_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QPrintDialog>
#include <QPrinter>
#include <QPrintPreviewDialog>

class QAction;
class QMenu;
class QComboBox;
class QFontComboBox;

class QMdiArea;
class ChildWnd;
class QMdiSubWindow;
class QSignalMapper;

class MyWord : public QMainWindow
{
    Q_OBJECT    
public:
    MyWord(QWidget *parent = nullptr);
    ~MyWord();
    
private:
    void createActions();
    void createToolBar();
    void createMenu();
    void createStatusBar();
    void enabledText(bool bEnable);
    
    ChildWnd *createChildWnd();     // 创建子窗口
    QMdiSubWindow *findChildWnd(const QString &fileName);
    
    void closeEvent(QCloseEvent *) override;
    
private slots:
    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void filePrint();
    void printPreview(QPrinter *);
    void filePrintPreview();
    void tileSubWindows();
    void cascadeSubWindows();
    void activateNextSubWindow();
    void activatePreviousSubWindow();
    void closeActiveSubWindow();
    void closeAllWindows();
    void setActiveSubWindow(QWidget *);
    void updateActiveWndAndActions();
    
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    
    void insert();
    void textBold();
    void textItalic();
    void textUnderline();
    void textAlign(QAction *);
    void textColor();
    void about();
    
    void closeAllSubWindows();
    
    void updateWindowMenu();
    void textStyle(int);
    void textFamily(const QString &);
    void textSize(const QString &);
    void onEditActionChecked();
    
    //void updateMenus();
    
private:
    QMdiArea    *_mdiArea;
    QMenu       *_fileMenu;
    QMenu       *_editMenu;
    QMenu       *_formatMenu;
    QMenu       *_fontMenu;
    QMenu       *_alignMenu;
    QMenu       *_windowMenu;
    QMenu       *_helpMenu;
    
    QToolBar    *_fileToolBar;
    QToolBar    *_editToolBar;
    QToolBar    *_formatToolBar;
    QToolBar    *_comboToolBar;
    QComboBox   *_comboStyle;
    QFontComboBox *_comboFont;
    QComboBox   *_comboSize;
    
    QAction     *_newAct;
    QAction     *_openAct;
    QAction     *_saveAct;
    QAction     *_saveAsAct;
    QAction     *_printAct;
    QAction     *_printPreviewAct;
    QAction     *_exitAct;
    
    QAction     *_undoAct;
    QAction     *_redoAct;
    QAction     *_cutAct;
    QAction     *_copyAct;
    QAction     *_pasteAct;
    
    QAction     *_boldAct;
    QAction     *_italicAct;
    QAction     *_underlineAct;
    QAction     *_leftAlignAct;
    QAction     *_centerAct;
    QAction     *_rightAlignAct;
    QAction     *_justifyAct;
    QAction     *_colorAct;
    QAction     *_insertAct;
    
    QAction     *_closeAct;
    QAction     *_closeAllAct;
    QAction     *_titleAct;
    QAction     *_cascadeAct;
    QAction     *_nextAct;
    QAction     *_previousAct;
    QAction     *_separatorAct;
    
    QAction     *_aboutAct;
    QAction     *_aboutQtAct;
    
private:
    QSignalMapper   *_wndMapper;
    ChildWnd        *_activeChildWnd = nullptr;
};
#endif // MYWORD_H
