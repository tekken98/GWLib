#include <iostream>
#include <vector>
#include <exception>
#include <memory>
#include "GWindow.h"
//#include <typeinfo>
const int ID_LIST       = 1001;
const int ID_DIALOG     = 2001;
const int ID_RUN        = 3001;
const int ID_MB         = 4001;
const int ID_PANEL      = 5001;
class mypanel : public GPanel 
{
    GButton *btn;
    GButton *btn1;
    GList *list;
    public:
    mypanel(){
        btn = new GButton("good");
        btn1 = new GButton("bad");
        list = new GList("list");
        list->addList("good");
        list->addList("bad");
        list->addList("bad one");
        add(btn);
        add(btn1);
        add(list);
        setTitle("MyPanel");
    }
    ~mypanel(){
        delete btn;
        delete btn1;
    }
};
class MW : public GWindowRun
{
    GList * m_list;
    mypanel * m_panel;
    public:
    void draw(){
        //GString s = m_list->getTitle() + "\n";
        //msg(s);
    }

    static void onBtn(const XEvent& ev){
        MW *p = (MW*)GgetWindowMap(ID_RUN);
        mypanel * mp = (mypanel*)GgetWindowMap(ID_PANEL);
            mp->setWindowRectInParent(0,0,200,200);
            GRect r = mp->getParent()->getWindowRectInParent();
            int x = (r.width() - mp->getWindowRectInParent().width()) / 2;
            mp->move(x,r.height());
            mp->recalcRect();
            mp->layout();
            mp->setVisible(true);
            mp->needDraw(true);
            mp->draw();
            mp->setFocus();
    }
    MW(){
        //setWindowRectInParent(0,0,400,400);
        m_panel = new mypanel();
        m_panel->addMap(ID_PANEL);
        GFrameLayout manager("Manager");
        GFrameLayout up("up frame"), 
                     mid("mid frame"), 
                     right("right frame"),
                    low("low frame"),
                     innerup("inner frame"),
                     innerdown("inner frame"),
                     left("left frame");

        GList list;
        GEdit ed("one"), ed2("two");

        list.addList("one");
        list.addList("two");
        GList apple;
        apple.addList("big");
        apple.addList("small");
        apple.addList("mid");
        m_list = &apple;
        up.add(&list);
        up.add(&apple);
        up.add(&ed);
        up.add(&ed2);
        GList l;
        GEdit label("good");
        l.addList("good");
        l.addList("bad");
        GListBox lb;
        GListBox bb;
        GString str[] {"one","two","three","four","five","six","seven"};
        for (int i = 0;i < 7;i++){
            l.addList(str[i]);
            bb.addList(str[i]);
            list.addList(str[i]);
        }
        GLabel lll("good");
        GLabel btn("button");
        char ** fonts;
        int font_count;
        fonts = XListFonts(g_base.getDisplay(),"*gb*",10000,&font_count);
        for (int i = 0; i< font_count; i++)
        {
            lb.addList(fonts[i]);
        }
        XFreeFontNames(fonts);


        mid.add(&lb);
        lb.setMaxLine(10);
        mid.add(&l);
        mid.add(&bb);
        mid.add(&label);
        mid.add(&lll);

       low.add(&left);
       low.add(&right);

       GButton mb("okman");
       mb.setFont();
       mb.addEventHandler(onBtn);
       mb.addMap(ID_MB);
       mb.add(m_panel);
            m_panel->setFixed(true);
            m_panel->showFrame(true);
        up.add(&mb);
        manager.addLayout(new GLayoutVertical());
        up.addLayout(new GLayoutVertical());
        mid.addLayout(new GLayoutHori());
        right.addLayout(new GLayoutVertical());
        left.addLayout(new GLayoutVertical());
        low.addLayout(new GLayoutHori());
        innerup.addLayout(new GLayoutVertical());
        innerdown.addLayout(new GLayoutVertical());
        left.add(&innerup);
        left.add(&innerdown);
        
        manager.add(&up);
        manager.add(&mid);
        manager.add(&low);
        addFrame(&manager);
        manager.setBorderWidth(0);
        manager.showStatus(true);
        run();
    }
    ~MW(){
        delete m_panel;
    }
};
int main () {
    MW w;
    return 0;
}
