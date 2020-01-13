#ifndef GWINDOW_H
#define GWINDOW_H
#include "foo.h"
#include <cassert>
#include "GString.h"
using std::string;
using std::cout;
using std::endl;
const int COLORBRIGHT = 0xcccccc;
const int COLORMID = 0x666666;
const int COLORDARK  = 0x111111;
const int COLORWHITE = 0xffffff;
const int COLORBLACK = 0x0;
const int COLORBLUE  = 0x0000ff;
const int COLORRED  = 0xff0000;
enum MSG { MSG_VOID=1, MSG_REDRAW,MSG_CONTINUE,MSG_NODRAW};
template <typename T>
void msg(T t){
    std::cout << t << std::endl;
}

class GPoint
{
    public:
        int x , y;
    public:
        //constructor
        GPoint():x(0),y(0){};
        //GPoint(const GPoint& s);
        GPoint(int x,int y ) : x(x),y(y){};
        GPoint& operator=(const GPoint& s){
            x = s.x;
            y = s.y;
        }
        //~GPoint();
    public:
        //maniulator
        GPoint& operator +=( const GPoint& p){
            x += p.x;
            y += p.y;
            return *this;
        }
        //accessor
        void dump(){
            cout << "GPoint dump: ";
            cout << "x:" << x << " y:" << y ;
        }
};
class GRect
{
    public:
        int x1,y1;
        int x2,y2;
    public:
        //constructor
        GRect():x1(0),y1(0),x2(0),y2(0){};
        GRect(const GRect& s){
            *this = s;
        };
        GRect(int x1 , int y1, int x2, int y2):x1(x1),y1(y1),x2(x2),y2(y2){};
        GRect& operator=(const GRect& s){
            x1 = s.x1;
            y1 = s.y1;
            x2 = s.x2;
            y2 = s.y2;
        };
        ~GRect(){};
    public:
        //maniulator
        void move(int x , int y){
            int w = width();
            int h = height();
            x1 = x;
            y1 = y;
            x2 = x1 + w;
            y2 = y1 + h;
        }
        void move(const GPoint& p){
            move(p.x,p.y);
        }
        void add(int x , int y){
            x1 += x;
            y1 += y;
            x2 += x;
            y2 += y;
        }
        void add(const GPoint & pt){
            x1 += pt.x;
            y1 += pt.y;
            x2 += pt.x;
            y2 += pt.y;
        }
        void scale(int s){
            x1 -= s;
            y1 -= s;
            x2 += s;
            y2 += s;
        }
        void shrink(int s) {
            x1 += s;
            y1 += s;
            x2 -= s;
            y2 -= s;
        }
        //accessor
        bool ptInRect(int x, int y){
            return  x > x1 && x < x2 && y > y1 && y < y2;
        }
        bool ptInRect(const GPoint& pt){
            return ptInRect(pt.x, pt.y);
        }
        int width() const { return x2 - x1 ;};
        int height() const { return y2 - y1;};
        void dump() const{
            cout << "GRect dump : ";
            cout << "x1:" << x1 << " y1:" << y1 ;
            cout << " x2:" << x2 << " y2:" << y2 ;
            cout << " width:"<< width() << " height:" << height();
            cout << endl;
        }
};
class GBase
{
    private:
        Display* m_dpy;
        Window m_win;
        GRect m_win_rect ;
        int m_screennum;
        int m_dpy_width;
        int m_dpy_height;
    public:
        //constructor
        GBase(){
            std::cout << "GBase"  << std::endl;
            m_dpy = XOpenDisplay(NULL);
            assert( m_dpy != NULL);
            m_screennum = DefaultScreen(m_dpy);
            m_dpy_width = DisplayWidth(m_dpy,m_screennum);
            m_dpy_height = DisplayHeight(m_dpy,m_screennum);
            m_win = XCreateSimpleWindow(m_dpy,XDefaultRootWindow(m_dpy),0,0,400,400,
                    20,20,whitePixel());
            XSelectInput(m_dpy,m_win,
                    ExposureMask|StructureNotifyMask
                    |SubstructureNotifyMask
                    |ButtonPressMask|ButtonReleaseMask
                    |PointerMotionMask|KeyPressMask
                    );
            m_win_rect = GRect(0,0,400,400);
        }
        GBase(const GBase& s){};
        //GBase& operator=(const GBase& s);
        ~GBase(){};
        //~GBase(){XCloseDisplay(m_dpy);};
    public:
        //maniulator
        void show() {
            std::cout << "show\n";
            XEvent e;
            /*
               while(XNextEvent(m_dpy,&e)){
               if (e.type == ExposureMask) break;
               }
               */
            XMapWindow(m_dpy,m_win);
            XFlush(m_dpy);
        }
        //accessor
        Display * getDisplay() { return m_dpy; }
        Window& getWindow() { return m_win;};
        unsigned long whitePixel(){ return WhitePixel(m_dpy,m_screennum);}
        unsigned long blackPixel(){ return BlackPixel(m_dpy,m_screennum);}
        void centerWindow(Window win){
            Window pw = win;
            if (pw == -1)
                pw = m_win;
            XMoveWindow(m_dpy,m_win, (m_dpy_width - m_win_rect.width()) / 2,
                    (m_dpy_height  - m_win_rect.height()) / 2);
        }
        GRect& getWindowRect(){
            return m_win_rect;
        }
        void setWindowRect(const GRect&r){
            m_win_rect = r;
        }


};
GBase g_base;
typedef void (* EVENT_HANDLER)(XEvent&);

class GEvent
{
    private:
        EVENT_HANDLER m_fun;
    public:
        //constructor
        GEvent():m_fun(NULL){};
        GEvent(const GEvent& s){};
        GEvent& operator=(const GEvent& s){};
        ~GEvent(){};
    public:
        //maniulator
        void addEventHandler(EVENT_HANDLER f){
            m_fun = f;
        };
        //accessor
        void doEvent(XEvent& ev){
            if (m_fun != NULL)
                (*m_fun)(ev);
        }
};
class GWindowBase
{
    private:
        GRect m_reservedrect;
        GWindowBase * mp_parent;
        GC m_gc;
        GRect m_rect;
        bool m_need_draw = true;
    public:
        //constructor
        GWindowBase():mp_parent(NULL),m_gc(NULL),m_rect(0,0,0,0){};
        //GWindowBase(const GWindowBase& s);
        //GWindowBase& operator=(const GWindowBase& s);
        //~GWindowBase();
    public:
        //maniulator
        //accessor
        bool isCrossRect(const GRect& r1, const GRect& r2){
            if ( r1.x1  >= r2.x2) return false;
            if ( r1.x2  <= r2.x1 ) return false;
            if ( r1.y1  >= r2.y2) return false;
            if ( r1.y2  <= r2.y1 ) return false;
            return true;
        }

        void setReservedRect(const GRect& r ){
            m_reservedrect = r;
        }
        void setReservedRect(){
            m_reservedrect = GRect(0,0,0,0);
        }
        GRect& getReservedRect(){
            return m_reservedrect;
        }
        void            setParent(GWindowBase * p){ mp_parent = p; };
        GWindowBase*    getParent(){ return mp_parent;};

        GPoint          getAbsolutePosition(){
            GWindowBase * base = mp_parent;
            GPoint pt = GPoint(m_rect.x1, m_rect.y1);
            GString s;
            if (base == NULL)
                return pt;
            pt += base->getAbsolutePosition(); 
            return pt;
        }
        GRect&          getWindowRect(){ return m_rect; }
        void            move(GPoint& p){ m_rect.move(p.x,p.y);}
        void            move(int x, int y ){ m_rect.move(x,y);}
        void            setWindowRect(const GRect& r){
            m_rect =  r;
        }
        void            setWindowRect(int x1,int y1,int x2, int y2)
        {
            m_rect.x1 = x1 ;
            m_rect.y1 = y1;
            m_rect.x2 = x2;
            m_rect.y2 = y2;
        }
        void            needDraw(bool b){ m_need_draw = b;}
        bool            needDraw(){return m_need_draw;}
        GC getGC(){
            if (m_gc == NULL){
                XGCValues values;
                unsigned long valuemask = 0;
                m_gc = XCreateGC(getDisplay(),getWindow(),valuemask,&values);
            }
            return m_gc;
        }


        Display * getDisplay(){ return g_base.getDisplay();};
        Window getWindow(){return g_base.getWindow();};

        void dump(){
            cout << "WindowBase  dump:"<<endl;
            if (mp_parent == NULL)
                cout << "m_parent = NULL" << endl;
            else 
                cout << "m_pparent = " << mp_parent << endl;
            m_rect.dump();
        }
        GRect getFontStringRect(const char* str) {
            int direction,ascent,descent;
            XCharStruct overall;
            XFontStruct * p = XQueryFont(getDisplay(),XGContextFromGC(getGC()));
            XTextExtents(p,(char*)str,strlen(str),&direction,&ascent,&descent,&overall);
            return GRect(0,descent,overall.width,(ascent+descent)+descent);
        }
        int getFontStringBaseline(const char* str){
            int r = getFontStringRect(str).y1;
            return r;
        }

        virtual void    update(){}; 
        virtual MSG     processEvent(XEvent& ev){};
        virtual void    draw() {};
        //virtual void    calcAll(){};
};
template <typename T>
class GWindow : public GWindowBase
{
    private:
        GString m_title;
        std::vector<GWindowBase*> m_pv_childs;
        GRect m_str_rect;
        int m_gap;
    public:
        //constructor
        GWindow():m_pv_childs(),m_title("no"),m_gap(10) {};
        GWindow(const string& s){
            m_title = s;
        }
        GWindow(const GWindow& s){};
        //GWindow& operator=(const GWindow& s);
        virtual ~GWindow(){};
    public:
        //maniulator
        void addFrame(GWindowBase * f){
            setWindowRect(g_base.getWindowRect());
            add(f);
            f->setWindowRect(g_base.getWindowRect());
            f->update();
        }
        void setGap(int g){
            m_gap = g;
        }
        void setBackground() { 
            XSetBackground(getDisplay(),getGC(),g_base.whitePixel());
        }
        void setForeground() {
            XSetBackground(getDisplay(),getGC(),g_base.blackPixel());
        }
        void setForeground(int color){
            XSetForeground(getDisplay(),getGC(),color);
        }
        void drawString(int x , int y , const char * str){
            GPoint pt = getAbsolutePosition();
            setForeground();
            XDrawString(getDisplay(), getWindow(), getGC(), 
                    pt.x + 2 + x, pt.y + y, str, strlen(str));
        }
        void drawStringCenter(const GRect& r,const char * str){
            GRect fr = getFontStringRect(str);
            int baseh = fr.y1;
            int x = 2;
            int y = r.y2 - ((r.height() - fr.height()) / 2) - baseh;
            drawString(x,y,str);
        }
        void drawRectangle(const GRect& cr){
            GRect r = cr;
            r.add(getAbsolutePosition());
            XDrawRectangle(getDisplay(),getWindow(),getGC(),r.x1,r.y1,r.width(),r.height());
        }
        void fillRectangle(const GRect& cr){
            fillRectangle(cr,COLORWHITE);
        }
        void fillRectangle(const GRect& cr, int color) {
            setForeground(color);
            GRect r = cr;
            r.add(getAbsolutePosition());
            XFillRectangle(getDisplay(),getWindow(),getGC(),r.x1, r.y1, r.width(),r.height());
        }
        void fillRectangle(int x1 , int y1, int x2, int y2){
            GPoint pt = getAbsolutePosition();
            fillRectangle(GRect(x1,y1,x2,y2).add(pt));
        }
        void add(GWindowBase * w ){
            m_pv_childs.push_back(w);
            w->setParent(this);
        }

        std::vector<GWindowBase*> getChilds() { return m_pv_childs;};
        void run()
        {
            XMapWindow(getDisplay(),getWindow());
            XEvent ev;
            g_base.centerWindow(-1);
            while(1){
                XNextEvent(getDisplay(),&ev);
                for (auto a : m_pv_childs){
                    switch(ev.type){
                        case Expose:
                            //case ConfigureNotify:
                            a->setWindowRect(0,0,ev.xexpose.width,ev.xexpose.height);
                            break;
                    }
                    a->processEvent(ev);
                }
            }
        }

        //accessor
        bool isForMe(XEvent& ev){
            return getWindowRect().ptInRect(ev.xbutton.x,ev.xbutton.y);
        }
        bool isButtonDown(XEvent& ev){
            return ev.type == ButtonPress;
        }
        bool isPointerMove(XEvent& ev){
            return ev.type == MotionNotify;
        }
        int xOfEv(XEvent& ev){
            return ev.xbutton.x;
        }
        int yOfEv(XEvent& ev){
            return ev.xbutton.y;
        }
        void drawLine(int beginx , int beginy, int endx, int endy){
            GPoint pt = getAbsolutePosition();
            XDrawLine(getDisplay(),getWindow(),getGC(), 
                    pt.x + beginx,pt.y + beginy,pt.x + endx,pt.y + endy);
        }
        void drawSolid(const GRect& cr, int color){
            fillRectangle(cr,color);
            //drawFrame(cr);
        }
        void drawFrame(const GRect& cr){
            GRect r = cr;
            //r.add(getAbsolutePosition());
            drawFrame(cr,COLORBRIGHT, COLORDARK); 
        }
        void drawFrame(const GRect& cr , int colorLeft, int colorRight){
            GRect r = cr;
            //r.add(getAbsolutePosition());
            setForeground(colorLeft);
            drawLine(r.x1,r.y1,r.x2,r.y1);
            drawLine(r.x1,r.y1 + 1,r.x2 - 1,r.y1 + 1);
            drawLine(r.x1,r.y1, r.x1, r.y2);
            drawLine(r.x1+1,r.y1, r.x1+1, r.y2 - 1);
            setForeground(colorRight);
            drawLine(r.x1+1,r.y2,r.x2,r.y2);
            drawLine(r.x1+2,r.y2 - 1,r.x2 - 1,r.y2 - 1);
            drawLine(r.x2,r.y1+1,r.x2,r.y2-1);
            drawLine(r.x2-1,r.y1+2,r.x2-1,r.y2-2);
        }
        void setTitle (const GString& s) { 
            m_title = s;
            m_str_rect =  getFontStringRect(s);
        };
        GRect& getStrRect() { return m_str_rect;}
        GString& getTitle(){return m_title;};
        void drawAllWindow(){
            for (auto a : m_pv_childs){
                a->needDraw(true);
                if (a->needDraw())
                    a->draw();
            }
        }
        void dump(){
            cout << " ############ " << endl;
            GWindowBase::dump();
            cout << "GWindow dump  " << endl;
            cout << "m_title :     " << m_title << endl;
            // cout << "m_str_rect:   " ;
            //m_str_rect.dump();
            cout << " ############ " << endl;
        }
};
class GEdit : public GWindow<GEdit>, public GEvent
{
    private:
        Cursor m_cursor;
        bool m_in = false;
        bool m_focus = false;
        int m_cursor_position = 1;
    public:
        // ev x y has changed to local window 
        MSG processEvent(XEvent& ev){ wchar_t buf[128];
            GRect r = getWindowRect();
            m_in = r.ptInRect(ev.xbutton.x, ev.xbutton.y) ;
            if (ev.type == ButtonPress ){
                if (m_in){
                    m_focus = true;
                    int gap = getFontStringRect(getTitle()).width() / strlen(getTitle());

                    int pos = (ev.xbutton.x - r.x1) / gap; 
                    if (pos < getTitle().size())
                        m_cursor_position = pos;
                    else
                        m_cursor_position = getTitle().size();

                    needDraw(true);
                }
                else{
                    if (m_focus == true){
                        m_focus = false;
                        needDraw(true);
                        draw();
                        msg("return coninue");
                        return MSG_CONTINUE;
                    }
                }
            }
            if ( ev.type == KeyPress && m_focus){
                //KeySym sym = XKeycodeToKeysym(getDisplay(),key,0);
                KeySym sym = XLookupKeysym(&ev.xkey,0);
                GString s = getTitle();
                s.insert(m_cursor_position,1,(char)sym);
                m_cursor_position++;
                GWindow::setTitle(s);
                //this->getParent()->calcAll();
                needDraw(true);
            }
            if (needDraw()){
                draw();
                return MSG_NODRAW;
            }
            return MSG_CONTINUE;
        }
        GEdit(const string&s) {
            setTitle(s);
        }
        void setTitle(const GString& s){
            GWindow::setTitle(s);
            getStrRect().scale(2);
            setWindowRect(getStrRect());
        }
#define XC_xterm 152
        void drawCursor(){
            GRect r = getWindowRect();
            r.move(0,0);
            GRect f = getFontStringRect(getTitle());
            int gap = f.width() / strlen(getTitle());
            int pos = 0;
            if ((m_cursor_position * gap) > r.width())
                pos = r.width() / gap;
            else
                pos = m_cursor_position;
            r.x1 = r.x1 + pos * gap + 1;
            r.x2 = r.x1 + 1;
            r.y1++;
            r.y2--;
            fillRectangle(r, COLORBLACK);
        }
        void drawFocus(){
            GRect r = getWindowRect();
            r.move(0,0);
            int height;
            height = r.height();
            int width = r.width();
            GString s = getTitle();
            int baseline = getFontStringBaseline(s);
            int strwidth = getFontStringRect(s).width();
            int w = getFontStringRect("a").width();
            if (strwidth > width) {
                int l = width / w;
                int b = 0;
                int gap = getFontStringRect(getTitle()).width() / strlen(getTitle());
                if ((m_cursor_position * gap) > r.width()){
                    b = r.width() / gap;
                    s = s.substr(m_cursor_position - b,l);
                }else
                    s = s.substr(0,l);
            }
            setForeground(0xcccccc);
            fillRectangle(r);
            drawFrame(r,0x111111,0xcccccc);
            setForeground(0x0);
            drawString(r.x1 ,r.y1 - 2 + height - baseline,s);
        }

        //maniulator
        void draw() {
            msg(getTitle() + "drawing");
            if (!needDraw())
                return;
            drawFocus();
            if (m_focus)
                drawCursor();
            needDraw(false);
        }
        //accessor
};
class GLabel : public GWindow<GLabel>, public GEvent
{
    public:
        GLabel(const string&s) {
            setTitle(s);
        }
        void setTitle(const string& s){
            GWindow::setTitle(s);
            setWindowRect(getStrRect());
            getWindowRect().scale(2);
        }
        void drawUpdate(){
            GRect r = getWindowRect();
            r.move(0,0);
            drawSolid(r,COLORMID);
            setForeground(COLORWHITE);
            drawStringCenter(r,getTitle());
        }

        //maniulator
        void draw() {
            drawUpdate();
            needDraw(false);
        }
        //accessor
};
class GMenuItem
{
    private:
        GString m_name;
        std::vector<GMenuItem> m_item;
    public:
        //constructor
        GMenuItem(){};
        GMenuItem(const GString& s){
            m_name = s;
        }
        GMenuItem(const GMenuItem& s){
            m_name = s.m_name;
            m_item = s.m_item;
        };
        GMenuItem& operator=(const GMenuItem& s){
            m_name = s.m_name;
            m_item = s.m_item;
        };
        ~GMenuItem(){};
    public:
        //maniulator
        void add(const GMenuItem& item){
            m_item.push_back(item);
        }
        void add(const GString& name){
            m_item.push_back(GMenuItem(name));
        }
        GString& getName(){
            return m_name;
        }
        auto getSub() const {
            return m_item;
        }
        bool haveSub() {
            return m_item.size() > 0;
        }
        //accessor
};
class GMenu : public GWindow<GMenu>
{
    private:
        std::vector<GMenuItem> m_item;
        static const  int m_width_gap = 10;
        static const  int m_height_gap = 10;
        bool m_show = false;
        bool m_showsub = false;
        int m_row = -1;
        int m_uprow = -1;
        int m_col = -1;
        std::vector<GRect> m_cur_rect;
        std::vector<GMenuItem> m_cur_item;
    public:
        //constructor
        GMenu(){};
        GMenu(const GMenu& s){};
        GMenu& operator=(const GMenu& s){};
        ~GMenu(){};
    public:
        //maniulator
        //accessor
        void drawRightArrow(GRect& r ){
            GRect w = getFontStringRect(">");
            // w.y1 is desent
            drawString(r.x2 - w.width(),r.y2 - w.height() / 2 - w.y1,">");
        }
        void removeSubRect(int index){
            auto itrect = m_cur_rect.end();
            auto ititem = m_cur_item.end();
            for (int r  = m_cur_rect.size() - 1;r > index; --r){
                clearDraw();
                m_cur_rect.erase(--itrect);
                m_cur_item.erase(--ititem);
                m_showsub = false;
            }
        }
        MSG showMenuMove(const XEvent& ev){
            int x = ev.xbutton.x, y = ev.xbutton.y;
            int count = -1;
            GString s ;

            for (int index  = m_cur_rect.size();index > 0;){
                count++;
                index--;
                GRect rect = m_cur_rect[index];
                GMenuItem item = m_cur_item[index];
                if (rect.ptInRect(x,y)){
                    int height = rect.height() / item.getSub().size();
                    int row = (y - rect.y1) / (height)  ;
                    int total_row =item.getSub().size(); 
                    if (ev.type == ButtonPress){
                        if (item.getSub()[row].haveSub())
                            return MSG_VOID;
                        clearAll();
                        m_show = false;
                        if (row < total_row){
                            GString name = (item.getSub()[row].getName());
                            GString s = "you press " + name;
                            msg(s);
                        }
                        return MSG_REDRAW;
                    }
                    if ( count >0 && m_row != -1)
                        removeSubRect(index);
                    if (count > 1 && m_row == -1)
                        removeSubRect(index);

                    if (row < total_row ) {
                        if (count >0){
                            if (m_uprow == row && m_row == -1)
                                return MSG_VOID;
                        }
                        if(count == 0)
                            if (row == m_row)
                                return MSG_VOID;
                        drawMenuAt(item,GPoint(rect.x1, rect.y1),row);
                        GMenuItem items = item.getSub()[row];
                        m_showsub = false;
                        m_row = row;
                        if (items.haveSub() && m_showsub == false){
                            drawMenuAt(items,GPoint(rect.x1 + rect.width(),rect.y1 + row * height),-1);
                            m_uprow = row;
                            m_showsub = true;
                            m_row = -1;
                            return  MSG_VOID;
                        }
                    } 
                }
            }
            { // showing menu and menu title move
                int menutitle_begin = 0;
                int cur_col = 0;
                for (auto a : m_item){
                    GRect r = getFontStringRect(a.getName());
                    r.move(menutitle_begin,0);
                    if (r.ptInRect(ev.xbutton.x,ev.xbutton.y)) {
                        if ( cur_col != m_col){
                            clearAll();
                            GPoint p(r.x1, r.y1 + r.height());
                            m_showsub = false;
                            needDraw(false);
                            getParent()->draw();
                            drawMenuAt(a,p,-1);
                            m_col = cur_col;
                            m_row = -1;
                            return  MSG_VOID;
                        }
                    }
                    cur_col++;
                    menutitle_begin += r.width() + m_width_gap;
                }
                if (ev.type == ButtonPress){
                    clearAll();
                    return MSG_REDRAW;
                }
            }
            return MSG_VOID;
        }

        MSG processEvent(XEvent& ev){
            int ex = ev.xbutton.x;
            int ey = ev.xbutton.y;
            GRect r ;
            if (m_show){
                return showMenuMove(ev);
            }
            // menu not showing
            int i = 0;
            int x = 0;
            if (ev.type == ButtonPress ) {
                for(auto a : m_item){
                    r = getFontStringRect(a.getName());
                    r.move(x,0);
                    if (r.ptInRect(ev.xbutton.x,ev.xbutton.y))
                    {
                        if (m_show == true && i != m_col){
                            clearDraw();
                        }
                        GPoint p(r.x1, r.y1 + r.height());
                        drawMenuAt(a,p,-1);
                        m_col = i;
                        return MSG_VOID;
                    }
                    i++;
                    x += r.width() + 10;
                }
                m_show = false;
                //clearDraw();
            }
            return MSG_VOID;
        }
        void clearDraw(){
            setForeground(0xffffff);
            GRect r = *(--m_cur_rect.end());
            r.y2++;
            r.x2++;
            fillRectangle(r);
            //needDraw(true);
            //draw();
        }
        void clearAll(){
            setForeground(0xffffff);
            for (auto a : m_cur_rect){
                a.y2++;
                a.x2++;
                fillRectangle(a);
            }
            m_cur_rect.clear();
            m_cur_item.clear();
            m_row = -1;
            m_uprow = -1;
            m_show = false;
            m_showsub =false;
        }

        void drawMenuAt(const GMenuItem& item, const GPoint& p,int row){
            int height = 0;
            int width = 0;
            int lineheight = 0;
            std::vector<GString> s;
            std::vector<bool> item_sub;
            GRect r ;
            for (auto a : item.getSub()) {
                r = getFontStringRect(a.getName());
                s.push_back(a.getName());
                if (a.haveSub())
                    item_sub.push_back(true);
                else
                    item_sub.push_back(false);
                height += r.height() + m_height_gap;
                lineheight = r.height();
                width = width < r.width()? r.width():width;
            }
            r = GRect(0,0,width+m_width_gap,height);
            r.move(p.x,p.y);
            if (row == -1 && m_showsub == false){
                m_cur_item.push_back(item);
                m_cur_rect.push_back(r);
            }
            fillRectangle(r,COLORMID);
            drawFrame(r,0x666666,0x111111);
            setForeground(COLORWHITE);
            int x = p.x, y = p.y + lineheight;
            int ry = p.y;
            int index=0;
            for (auto a : s){
                GRect itemRect(x , ry, x + r.width(),ry + lineheight+m_height_gap);
                if (index == row){
                    fillRectangle(itemRect, COLORBLUE);
                    setForeground(0xcccccc);
                }
                drawString(x,y,a);
                if (item_sub[index] == true)
                    drawRightArrow(itemRect);
                index++;
                y += lineheight + m_height_gap;
                ry += lineheight + m_height_gap;
            }
            setForeground(0x0);
            m_show = true;
        }
        void draw(){
            int x = 0;
            setForeground(COLORWHITE);
            for (auto a : m_item){
                GRect r = getFontStringRect(a.getName());
                drawString(x,r.height(),a.getName());
                x += r.width() + 10;
            }
        }
        void add(GMenuItem& item){
            m_item.push_back(item);
        }
};
class GList  :public  GWindow<GList>
{
    private:
        std::vector<GString> m_string_list;
        GRect m_showStringRect;
        bool m_btn_down = false;
        int m_select = 0;
        int row = 0;
        int oldrow = -1;
    public:
        //constructor
        GList (){ setWindowRect(0,0,400,20);}
        //GList (const GList & s);
        //GList & operator=(const GList & s);
        ~GList (){};
    public:
        //maniulator
        void addList(const GString& s){
            m_string_list.push_back(s);
        }
        void drawDownArrow(GRect& r){
            setForeground(COLORBLACK);
            int x = r.x2 - 10 - 5;
            int y = (r.height() - 5) / 2 ;
            int ex = r.x2 -5 ;
            for (int i = 0;i<6;i++){
                drawLine(x,y, ex, y);
                x++,y++,ex--;
            }
        }
        void draw(){
            if (!needDraw())
                return ;
            GRect r = getWindowRect();
            r.move(0,0);
            drawSolid(r,COLORWHITE);
            drawFrame(r);
            drawDownArrow(r);
            drawStringCenter(r,getTitle());
            needDraw(false);
        }
        MSG processEvent(XEvent& ev){
            if ((m_btn_down) &&  isPointerMove(ev)){
                int size = m_string_list.size();
                int space = m_showStringRect.height() / size;
                int by = getWindowRect().y2;
                int bx = m_showStringRect.x1;
                int ex = m_showStringRect.x2;
                int y = yOfEv(ev) - by;
                by -= getWindowRect().y1;
                if (y > 0) {
                    row = y  / space;
                    if  (row >= 0 && row  < size && row != oldrow){
                        showString();
                        GRect r (bx,row * space + by, 
                                ex,by + row * space + space);
                        showHighlightBar(r,COLORBLUE,m_string_list[row]);
                        m_select = row ;
                        oldrow = row;
                    }
                }
                return MSG_NODRAW;
            }
            if(m_btn_down  && isButtonDown(ev)){
                m_btn_down = false;
                oldrow= -1;
                setTitle(m_string_list[m_select]);
                setReservedRect();
                needDraw(true);
                return MSG_REDRAW;
            }
            if (ev.type == Expose){
                m_btn_down = false;
                oldrow= -1;
                needDraw(true);
                return MSG_REDRAW;
            }

            if (isForMe(ev) && isButtonDown(ev)){
                if (m_btn_down == true) {
                    m_btn_down = false;
                    return MSG_REDRAW;
                }
                else{
                    showString();
                    m_btn_down = true;
                    return MSG_NODRAW;
                }
            }
            return MSG_CONTINUE;
        }
        void showString(){
            GRect r = getWindowRect();
            showStringRect(m_string_list,GPoint(0,r.height()), COLORWHITE);
        }
        void showStringRect(std::vector<GString>&v , const GPoint& p , int solidColor){
            GRect r;
            GRect sr;
            int w = 0,h = 0;
            for(auto a : v){
                sr = getFontStringRect(a);
                w = sr.width() > w ? sr.width() : w;
                h += sr.height();
            }
            r.x2 = getWindowRect().width();
            r.y2 = getWindowRect().height() * v.size();
            r.move(p);
            m_showStringRect = r;
            setReservedRect(r);
            drawSolid(r,solidColor);
            drawFrame(r);
            setForeground(0x0);
            h = getWindowRect().height();
            for (auto a : v){
                sr = getFontStringRect(a);
                h += getWindowRect().height();
                drawString(p.x + 2, h - sr.y1, a);
            }
        }
        void showHighlightBar(const GRect& r,int highlighcolor, const GString& showStr){
            GRect rr = r;
            drawSolid(rr,highlighcolor);
            setForeground(0x0);
            drawStringCenter(rr,showStr);
        }
        //accessor
};
class GStatus : public GWindow<GStatus>
{
    private:
    public:
        //constructor
        GStatus(){setTitle("Status");};
        GStatus(const string& s ){
            setTitle(s);
        };
        GStatus(const char* s){
            setTitle(s);
        }
        //GStatus(const GStatus& s);
        //GStatus& operator=(const GStatus& s);
        //~GStatus();
    public:
        //maniulator
        void draw(){
            GRect r = getWindowRect();
            r.move(0,0);
            GRect r1;
            r1.x1 = r.width() / 3;
            r1.y1 = r.y1;
            r1.x2 = r1.x1 + 2;
            r1.y2 = r.y2;
            drawSolid(r,COLORMID);
            drawFrame(r,COLORWHITE,COLORBLACK);
            /*
               drawFrame(r1,COLORWHITE,COLORBLACK);
               r1.move(r.width() / 3 * 2,r1.y1);
               drawFrame(r1,COLORWHITE,COLORBLACK);
               */
            setForeground(COLORWHITE); 
            drawStringCenter(r,getTitle());
        }
        //accessor
};
class GLayoutVertical;
class GLayout;
class GFrame : public GWindow<GFrame>, public GEvent
{
    private:
        GStatus m_StatusBar;
        GMenu* m_menu = NULL;
        bool m_init = false;
        int m_gap = 10;
        GWindowBase* m_targetWin = NULL;
    public:
        virtual void draw() {
            drawFrame();
            if(m_menu != NULL){
                m_menu->draw();
            }
            drawAllWindow();
        }
        void innerdraw(){
            draw();
        }
        void redraw(){
            for (auto a : getChilds())
                if (a->needDraw())
                    a->draw();
        }
    public:
        GFrame():m_StatusBar("Status"){
            setTitle("Frame");
        }
        GFrame(const GString& s){
            setTitle(s);
        }
        ~GFrame(){
        }
        GStatus * getStatusBar(){ return &m_StatusBar;};
        void addMenu(GMenu * m){
            m_menu = m;
            m->setParent(this);
        }
        void invalidRect(const GRect& r){
            for(auto a : getChilds()){
                GRect cr = a->getWindowRect();
                if (isCrossRect(r, cr)){
                    a->draw();
                }
            }
        }
        void updateStatus(){
            GRect rootrect = getWindowRect();
            int height = m_StatusBar.getFontStringRect(getTitle()).height();
            m_StatusBar.setWindowRect(rootrect.x1, rootrect.y2 - height -2, rootrect.x2, rootrect.y2 + 2);
        }
        void update() {
            updateStatus();
            int x = 0;
            int y = 0;
            GRect r = getWindowRect();
            int h = 0;
            if (m_menu != NULL){
                h = getFontStringRect(getTitle()).height();
                y = h;
                m_menu->setWindowRect(r.x1,r.y1, r.x2, r.y1+h);
            }
            for (auto a : getChilds()){
                /*
                   GRect& r = a->getWindowRect();
                   r.move(x,y + 2);
                   int cx = a->getWindowRect().width();
                   x += cx + m_gap; 
                   y = a->getWindowRect().height();
                   */
            }
            if (!m_init){
                m_init = true;
            }
        }
        bool isReserved(){
            GRect r = getWindowRect();
            for (auto a : getChilds()){
                if (isCrossRect(r, a->getReservedRect())) {
                    return true;
                }
            }
            return false;
        }
        void drawFrame(){
            GRect r = getWindowRect();
            if (isReserved())
                return;
            drawSolid(r,COLORMID);
            GWindow::drawFrame(r);
            m_StatusBar.draw();
        }
        void drawStatus(){
            GRect r = m_StatusBar.getWindowRect();
            //msg(getTitle());
            if (isReserved())
                return;
            m_StatusBar.draw();
        }
        MSG innerprocessEvent(XEvent& ev){
            drawStatus();
            /*
               if (m_menu != NULL){
               if (m_menu->processEvent(ev) == MSG_REDRAW){
               draw();
               return MSG_REDRAW;
               }
               }
               */
            XEvent e = ev;
            GString s;
            int x = getAbsolutePosition().x;
            int y = getAbsolutePosition().y;
            int xx = e.xbutton.x - x;
            int yy = e.xbutton.y - y ;
            s.format("%d,%d,%s",xx , yy,(const char*)getTitle());
            m_StatusBar.setTitle(s);
            e.xbutton.x = xx;
            e.xbutton.y = yy;

            if (m_targetWin !=NULL){
                switch (m_targetWin -> processEvent(e)){
                    case MSG_NODRAW:
                        return MSG_NODRAW;
                    case MSG_REDRAW:
                        draw();
                        return MSG_REDRAW;
                    default:
                        break;
                }
            }
            for (auto a : getChilds()){
                switch(a->processEvent(e)){
                    case MSG_REDRAW:
                        draw();
                        m_targetWin = a;
                        return MSG_REDRAW;
                    case MSG_NODRAW:
                        m_targetWin = a;
                        return MSG_NODRAW;
                    case MSG_CONTINUE:
                        continue;
                        break;
                }
                m_targetWin = NULL;
            }
            return MSG_CONTINUE;
        }
};

class GLayout 
{
    private:
        GFrame * m_frame;
    public:
        GLayout(GFrame * f){ m_frame = f;};
        GLayout(){};
        GFrame * getFrame(){ return m_frame;};
        void setFrame(GFrame *f) {
            m_frame = f;
        }
        virtual void layout(){};
};
class GLayoutHori : public GLayout
{
    public:
        void layout(){
            GFrame * f = getFrame();
            int ct =f->getChilds().size();
            if  ( ct == 0)
                return;
            GRect r = f->getWindowRect();
            f->updateStatus();
            GStatus * status  = f->getStatusBar();
            int h = (r.height() - status->getWindowRect().height());
            int w = r.width() / ct;
            int x = 0;
            int y = 0;
            for (auto a : f->getChilds()){
                a->setWindowRect(x,y,w + x,h); 
                //a->update();
                x += w;
                //a->dump();
            }
        }

};
class GLayoutVertical : public GLayout
{
    public:
        GLayoutVertical(){};
        void layout(){
            GFrame * f = getFrame();
            int w =f->getChilds().size();
            if  ( w == 0)
                return;
            GRect r = f->getWindowRect();
            f->updateStatus();
            GStatus * status  = f->getStatusBar();
            int h = (r.height() - status->getWindowRect().height()) / w;
            int x = 0;
            int y = 0;
            for (auto a : f->getChilds()){
                a->setWindowRect(x,y,r.width(),y + h); 
                a->update();
                y += h;
                //a->dump();
            }
        }
};

class GFrameLayout : public GFrame
{
    private :
        GLayout * m_layout;
    public:
        GFrameLayout(const GString& s) : GFrame(s){};
        GFrameLayout(){
            m_layout = new GLayoutVertical();
        }
        ~GFrameLayout(){
            if (m_layout != NULL)
                delete m_layout;
        }
        void addLayout(GLayout * l){
            m_layout = l;
            l->setFrame(this);
        }
        MSG processEvent(XEvent& ev){
            if (ev.type == Expose && ev.xexpose.count == 0){
                m_layout->layout();
                innerdraw();
                for (auto a : getChilds()){
                    a->processEvent(ev);
                }
                return MSG_NODRAW;
            }
            return innerprocessEvent(ev);
        }
};
#endif
