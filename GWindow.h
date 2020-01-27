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
enum MSG {
    MSG_VOID=1, MSG_REDRAW,MSG_CONTINUE,MSG_NODRAW,
    MSG_SELECTED,MSG_NOSELECTED};
#include <iostream>
using namespace std;

void msg(){};
template <typename T, typename... V>
void  msg( T& a , V& ... args)
{
    cout << a ;
    msg(args...);
    std::cout.flush();
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
            return *this;
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
            cout << "x:" << x << " y:" << y ;
            cout << endl;
            cout.flush();
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
            return *this;
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
        GRect& add(int x , int y){
            x1 += x;
            y1 += y;
            x2 += x;
            y2 += y;
            return *this;
        }
        GRect& add(const GPoint & pt){
            x1 += pt.x;
            y1 += pt.y;
            x2 += pt.x;
            y2 += pt.y;
            return *this;
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
            cout << "x1:" << x1 << " y1:" << y1 ;
            cout << " x2:" << x2 << " y2:" << y2 ;
            cout << " width:"<< width() << " height:" << height();
            cout << endl;
        }
};
class GWindowBase;
class GBase
{
    private:
        Display* m_dpy;
        Window m_win;
        GRect m_win_rect ;
        std::map<int,GWindowBase*> m_windowMap;
        int m_screennum;
        int m_dpy_width;
        int m_dpy_height;
        GString m_window_name;
    public:
        //constructor
        GBase(){
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
        ~GBase(){
           if (m_dpy != NULL)
               XCloseDisplay(m_dpy);
        };
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
        Display *       getDisplay(){ return m_dpy; }
        Window&         getWindow() { return m_win;};
        unsigned long   whitePixel(){ return WhitePixel(m_dpy,m_screennum);}
        unsigned long   blackPixel(){ return BlackPixel(m_dpy,m_screennum);}
        void            centerWindow(Window win){
            Window pw = win;
            if (pw == -1)
                pw = m_win;
            XMoveWindow(m_dpy,m_win, (m_dpy_width - m_win_rect.width()) / 2,
                    (m_dpy_height  - m_win_rect.height()) / 2);
        }
        inline const GRect& getTopWindowRect(){
            return m_win_rect;
        }
        void setTopWindowRect(const GRect&r){
            m_win_rect = r;
        }
        void addMap(int id, GWindowBase * win){
           m_windowMap.insert(make_pair(id,win)); 
        }
        GWindowBase * getMap(int id){
            return m_windowMap[id];
        }
};
GBase g_base;
GWindowBase * GgetWindowMap(int id){
    return g_base.getMap(id);
};
typedef void (* EVENT_HANDLER)(const XEvent&);

class GEvent
{
    private:
        EVENT_HANDLER m_fun;
    public:
        //constructor
        GEvent():m_fun(NULL){};
        GEvent(const GEvent& s){};
        //GEvent& operator=(const GEvent& s){};
        ~GEvent(){};
    public:
        //maniulator
        void addEventHandler(EVENT_HANDLER f){
            m_fun = f;
        };
        //accessor
        void doEvent(const XEvent& ev){
            if (m_fun != NULL)
                (*m_fun)(ev);
        }
};
class GWindowBase
{
    private:
        GWindowBase * m_parent;
        GString m_title;
        GC m_gc;
        GRect m_window_rect;
        GRect m_client_rect;
        GRect m_reservedrect;
        GRect m_title_rect;
        static XFontStruct* mp_font_struct;
        int m_border_width = 2;
        bool m_need_draw = true;
    public:
        //constructor
        GWindowBase():m_parent(NULL),m_title("NoName"), m_gc(NULL),m_window_rect(0,0,0,0),m_reservedrect(0,0,0,0){};
        GWindowBase(const GString &s){ m_title = s;};
        //GWindowBase(const GWindowBase& s);
        //GWindowBase& operator=(const GWindowBase& s);
        ~GWindowBase(){
            if (m_gc != NULL){
                XFreeGC(g_base.getDisplay(),m_gc);
            }
            if ( mp_font_struct != NULL){
                //XFreeFont(g_base.getDisplay(),mp_font_struct);
                mp_font_struct = NULL;
            }
        };
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
        void            setParent(GWindowBase * p){ m_parent = p; };
        GWindowBase*    getParent(){ return m_parent;};

        // child window rect  is at parent's xy
        GPoint          getAbsolutePosition(){
            GPoint pt = GPoint(m_window_rect.x1, m_window_rect.y1);
            if (m_parent == NULL)
                return pt;
            pt += m_parent->getAbsolutePosition(); 
            return pt;
        }
        XEvent xevevtToContainer(const XEvent& e){
            XEvent ev = e;
            int x = getAbsolutePosition().x;
            int y = getAbsolutePosition().y;
            GRect r = getWindowRectInParent();
            ev.xbutton.x -= x - r.x1;
            ev.xbutton.y -= y - r.y1;
            return ev;
        }
        int             getBorderWidth(){ return m_border_width;}
        void            setBorderWidth(int w){ m_border_width = w;}
        // relative to Parent window
        GRect&          getWindowRectInParent(){ return m_window_rect; }
        // relative to Parent window
        GRect&          getClientRectInParent(){ return m_client_rect; }
        //relative to Current window
        GRect           getClientRectInWindow(){
            GRect r = m_client_rect;
            r.move(m_client_rect.x1 - m_window_rect.x1,
                    m_client_rect.y1 - m_window_rect.y1);
            return  r;
        }


        void            move(GPoint& p){ m_window_rect.move(p.x,p.y);}
        void            move(int x, int y ){ m_window_rect.move(x,y);}
        void            setWindowRectInParent(const GRect& r){ m_window_rect =  r; }
        void            setclientRectInparent(const GRect& r){ m_client_rect = r;}
        void            setclientRectInparent(int x1, int y1, int x2, int y2){
            m_client_rect.x1 = x1;
            m_client_rect.y1 = y1;
            m_client_rect.x2 = x2;
            m_client_rect.y2 = y2;
        }
        void            setWindowRectInParent(int x1,int y1,int x2, int y2)
        {
            m_window_rect.x1 = x1 ;
            m_window_rect.y1 = y1;
            m_window_rect.x2 = x2;
            m_window_rect.y2 = y2;
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
        void setTitle (const GString& s) { 
            m_title = s;
            m_title_rect =  getFontStringRect(s);
        };
        GRect& getTitleRect() { return m_title_rect;}
        GString& getTitle(){return m_title;};
 

        void dump(){
            if (m_parent == NULL)
                return ;
            else 
                m_parent->dump();
            cout << "m_title:" << m_title << endl;
            m_window_rect.dump();
        }
        GRect getFontStringRect(const char* str) {
            int direction,ascent,descent;
            XCharStruct overall;
            //const char * fontname = "";
            //XFontStruct * p = XLoadQueryFont(getDisplay(),fontname);
            //XFontStruct * p = XQueryFont(getDisplay(),XGContextFromGC(getGC()));
            XQueryTextExtents(getDisplay(),XGContextFromGC(getGC()),
                (char*)str,strlen(str),&direction,&ascent,&descent,&overall);
            //XFreeFont(getDisplay(),p);
            return GRect(0,descent,overall.width,(ascent+descent)+descent);
        }
        int getFontStringBaseline(const char* str){
            int r = getFontStringRect(str).y1;
            return r;
        }
        int getFontStringHeight(){
            return getFontStringRect("aA").height();
        }

        virtual void    recalcRect(){ 
            GRect  r = getWindowRectInParent(); 
            r.shrink(getBorderWidth()); 
            setclientRectInparent(r);
            }; 
        virtual MSG     processEvent(const XEvent& e){return MSG_CONTINUE;};
        virtual void    draw() = 0;
        virtual void    layout() {};
};
XFontStruct* GWindowBase::mp_font_struct = NULL;

template <typename T>
class GWindow : public GWindowBase
{
    private:
        std::vector<GWindowBase*> m_pv_childs;
        bool m_focused = false;
    public:
        //constructor
        GWindow():m_pv_childs(){};
        GWindow(const string& s):GWindowBase(s){
        }
        GWindow(const GWindow& s){};
        //GWindow& operator=(const GWindow& s);
        virtual ~GWindow(){};
    public:
        //maniulator
        void centerWindow(const GRect& parentRect, GRect& thisRect){
            int cx = (parentRect.width() - thisRect.width()) / 2;
            int cy = (parentRect.height() - thisRect.height()) / 2;
            thisRect.move(parentRect.x1 + cx, parentRect.y1 + cy);
        }
        void addMap(int id){
            g_base.addMap(id,this);
        }
        GWindowBase * getmap(int id){
            return g_base.getMap(id);
        }
        void setWindowName(const GString& s){
            XStoreName(g_base.getDisplay(),g_base.getWindow(),
                    s);
        }
        void addFrame(GWindowBase * f){
            //setWindowRectInParent(g_base.getTopWindowRect());
            add(f);
            //f->setWindowRectInParent(g_base.getTopWindowRect());
            //f->recalcRect();
        }
        void setBackground(int color) { 
            XSetBackground(getDisplay(),getGC(),color);
        }
        void setForeground(int color ) {
            XSetForeground(getDisplay(),getGC(),color);
        }
        void setForeground() {
            XSetForeground(getDisplay(),getGC(),g_base.blackPixel());
        }
        
        void setBackground() { 
            XSetBackground(getDisplay(),getGC(),g_base.whitePixel());
        }
        void drawString(int x , int y , const char * str){
            GPoint pt = getAbsolutePosition();
            //assert(getClientRectInWindow().ptInRect(x,y));
            XDrawString(getDisplay(), getWindow(), getGC(), 
                    pt.x + 2 + x, pt.y + y, str, strlen(str));
        }
        void drawString(const GPoint& p, const char * str){
            drawString(p.x, p.y, str);
        }
        void drawStringCenterY(const GRect& r,const char * str){
            GPoint p = getStringCenterXY(r,str);
            drawString(p,str);
        }
        GPoint getStringCenterXY(const GRect& r, const char * str){
            GRect fr = getFontStringRect(str);
            int baseh = fr.y1;
            unsigned int x; 
            x = (r.width() - fr.width())/2 ;
            int y = r.y2 - ((r.height() - fr.height()) / 2) - baseh;
            return GPoint(x,y);
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
            GRect r = GRect(x1,y1,x2,y2);
            fillRectangle(r);
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
            GRect r;
            timeval tv;
            fd_set fd;
            int connectfd;
            connectfd=ConnectionNumber(getDisplay());
            g_base.centerWindow(-1);
            Atom wmDelete = XInternAtom(getDisplay(),
                    "WM_DELETE_WINDOW",true);
            XSetWMProtocols(getDisplay(),getWindow(),&wmDelete,1);
            while(1){
                FD_ZERO(&fd);
                FD_SET(connectfd,&fd);
                tv.tv_usec = 0;
                tv.tv_sec = 1;
                int num = select(connectfd + 1,
                        &fd,NULL,NULL,&tv);
                if (num >0){
                    //msg("receive event \n");
                }else if (num == 0){
                    //msg("time fired \n");
                }else {
                    msg("quit \n");
                    return;
                }
                while(XPending(getDisplay())) {
                XNextEvent(getDisplay(),&ev);
                for (auto a : m_pv_childs){
                    switch(ev.type){
                        case Expose:
                            //case ConfigureNotify:
                            r = GRect(0,0,ev.xexpose.width,ev.xexpose.height);
                            a->setWindowRectInParent(r);
                            //a->recalcRect();
                            break;
                        case DestroyNotify:
                            msg("quit\n");
                            return;
                        case ClientMessage:
                            XUnmapWindow(getDisplay(),getWindow());
                            msg("quit\n");
                            return;
                    }
                    a->processEvent(ev);
                }
                }
                //break;
            }
        }
 
        void showHighlightBar(const GRect& r,int highlighcolor, const GString& showStr){
            GRect rr = r;
            drawSolid(rr,highlighcolor);
            setForeground(COLORWHITE);
            drawStringCenterY(rr,showStr);
        }
        GRect getDrawStringRect(std::vector<GString>&v,int from, int to){
            GRect r;
            GRect sr = getFontStringRect("aA");

            int w = 0,h = 0;
            int l = to > v.size() ? v.size():to;
            for(int i = from; i < l; i++){
                    w = sr.width() > w ? sr.width() : w;
                    h += sr.height();
            }
            r.x2 = getClientRectInWindow().width();
            r.y2 =sr.height() * (l - from);
            return r;
        }
 
        GRect  showStringRect(std::vector<GString>&v , const GPoint& p , int solidColor,int from , int to){
            GRect r = getDrawStringRect(v,from,to);
            r.move(p);
            setReservedRect(r);
            drawSolid(r,solidColor);
            drawFrame(r);
            setForeground(0x0);
            int h = p.y;
            int string_height = getFontStringHeight();
            int string_baseline = getFontStringBaseline("aA");
            int l = to > v.size() ? v.size():to;
            for (int i = from; i<l; i++){
                h +=string_height;
                drawString(p.x + 2, h - string_baseline, v[i]);
            }
            return r;
        }
 
        //accessor
        // at containner xy
        bool isForMe(const XEvent& ev){
            return getWindowRectInParent().ptInRect(ev.xbutton.x,ev.xbutton.y);
        }
        bool isButtonDown(const XEvent& ev){
            return ev.type == ButtonPress;
        }
        bool isButtonRelease(const XEvent& ev){
            return ev.type == ButtonRelease;
        }
        bool isPointerMove(XEvent& ev){
            return ev.type == MotionNotify;
        }
        bool isFocused(){
            return m_focused;
        }
        void isFocused(bool t){
            m_focused = t;
        }
        bool isExposed(const XEvent& ev){
            return  ev.type == Expose;
        }
        int xOfEv(XEvent& ev){
            return ev.xbutton.x;
        }
        int yOfEv(const XEvent& ev){
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
        void drawAllWindow(){
            for (auto a : m_pv_childs){
                a->needDraw(true);
                a->draw();
            }
        }
};

class GScrollBar : public GWindow<GScrollBar> 
{
    bool m_buttonPressing = false;
    int  m_moveheight = 5;
    GPoint m_pos;
    public:
        void draw(){
            if (!needDraw())
                return;
            GRect r = getWindowRectInParent();
            r.move(0,0);
            drawSolid(r,COLORRED); 
        };
        bool isButtonPressing() {
            return m_buttonPressing;
        }
        void setMoveHeight(int h){
            m_moveheight = h;
        }
        MSG processEvent(const XEvent& e){
            XEvent ev = xevevtToContainer(e);
            if (isForMe(ev) ){
                switch (ev.type){
                    case ButtonPress:
                        if (m_buttonPressing == false){
                            m_buttonPressing = true;
                            m_pos.x = e.xbutton.x;
                            m_pos.y = e.xbutton.y;
                        }
                }
            }
            if (isButtonPressing()){
                        GRect r = getWindowRectInParent();
                        GRect rp = getParent()->getClientRectInWindow();
                        int y = e.xbutton.y;
                        int by = r.y1;
                switch (ev.type){
                    case MotionNotify:
                        y = y - m_pos.y;
                        m_pos.y = e.xbutton.y;
                        by += y;
                        if ( by >= rp.y1 && by < rp.y1 + m_moveheight - r.height()){
                            r.move(r.x1, by);
                            setWindowRectInParent(r);
                            needDraw(false);
                            getParent()->needDraw(true);
                            getParent()->draw();
                            needDraw(true);
                            draw();
                        }
                        break;
                    case ButtonRelease:
                        m_buttonPressing = false;
                        break;
                }
                return MSG_NODRAW;
            }
            return MSG_CONTINUE;
        }
};

class GButton : public GWindow<GButton> , public GEvent
{
    GRect m_default_rect ;
    public:
        GButton(const GString& s) : m_default_rect{0,0,50,30}{
            setTitle(s);
        }
        virtual void layout(){
            
        }
        virtual void    recalcRect(){
            GRect p = getWindowRectInParent();
            //p.add(b,b);
            GRect r = m_default_rect;
            GRect fr = getFontStringRect(getTitle());
            fr.move(0,0);
            GRect d;
            d = r;
            if (r.width() < fr.width()){
                d.x2 = fr.x2 + 10;
            }
            centerWindow(p,d);
            setWindowRectInParent(d);
        }; 
        virtual MSG     processEvent(const XEvent& e){
            if( isExposed(e)){
                needDraw(true);
                draw();
                return MSG_CONTINUE;
            }
            XEvent ev = xevevtToContainer(e);
            if (isForMe(ev)){
                if (isButtonDown(ev)){
                    onButtonDown(ev);
                    return MSG_NODRAW;
                }
                if (isButtonRelease(ev)){
                    onButtonUp(ev);
                    return MSG_NODRAW;
                }
            }
            return MSG_CONTINUE;
        };
        virtual void    draw() {
            if (needDraw()){
                GRect r = getWindowRectInParent();
                r.move(0,0);
                drawSolid(r,COLORMID);
                drawFrame(r, COLORWHITE,COLORBLACK);
                drawStringCenterY(r,getTitle());
            }
            needDraw(false);
        };
        void drawDown(){
                GRect r = getWindowRectInParent();
                r.move(0,0);
                drawFrame(r,COLORBLACK, COLORWHITE);
                drawStringCenterY(r,getTitle());
        }
        void drawUp(){
                GRect r = getWindowRectInParent();
                r.move(0,0);
                drawFrame(r, COLORWHITE,COLORBLACK);
                drawStringCenterY(r,getTitle());
        }
        void onButtonDown(const XEvent& ev){
            drawDown();
            doEvent(ev);
        }
        void onButtonUp(const XEvent& ev){
            drawUp();
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
        MSG processEvent(XEvent& e){ wchar_t buf[128];
            XEvent ev = xevevtToContainer(e);
            if (ev.type == Expose){
                draw();
                return MSG_CONTINUE;
            }

            GRect r = getClientRectInParent();
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
                // no other window draw only self draw in this process
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
            getTitleRect().scale(2);
            setWindowRectInParent(getTitleRect());
        }
#define XC_xterm 152
        void drawCursor(){
            GRect r = getClientRectInWindow();
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
            GRect r = getClientRectInWindow();
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
                    if (m_cursor_position -b < l)
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
            setWindowRectInParent(getTitleRect());
            getClientRectInParent().scale(2);
        }
        void drawUpdate(){
            GRect r = getClientRectInWindow();
            drawSolid(r,COLORMID);
            drawFrame(r,COLORRED,COLORRED);
            setForeground(COLORWHITE);
            drawStringCenterY(r,getTitle());
        }

        //maniulator
        void draw() {
            drawUpdate();
            needDraw(false);
        }
        MSG processEvent(XEvent& ev){
            if (ev.type == Expose){
                draw();
                return MSG_NODRAW;
            }
            return MSG_CONTINUE;
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
            return *this;
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
        GMenu& operator=(const GMenu& s){return *this;};
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

        MSG processEvent(XEvent& e){
            XEvent ev = xevevtToContainer(e);
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

class GListBox : public GWindow<GListBox>
{
    GRect m_top;
    GRect m_vertbar;
    GRect m_horibar;
    int m_top_height = 2;
    int m_item_count = 1;
    int m_item_length = 1;
    std::vector<GString> m_strings;
    GRect m_showStringRect;
    int m_maxline = 5;
    int m_string_height=0;
    int m_select = -1;
    int m_firstrow = 0;
    bool m_autoScroll=false;
    GScrollBar m_scrollBarVert;
    public:
    GListBox(){
        m_scrollBarVert.setParent(this);
        m_string_height = getFontStringRect("aA").height();
    };
    int getDefaultStringHeight(){
        int h = m_strings.size() > m_maxline ? m_maxline : m_strings.size(); 
        return h * getFontStringHeight();
    }
    GString getSelected(){
        if (m_select != -1)
            return m_strings[m_select];
        else
            return "";
    }
    int setMaxline(int m){
        m_maxline = m;
        return m;
    }
    int setSelected(int w){
        if (w >=0 && w < m_strings.size()){
            m_select = w;
            return w;
        }else
            return -1;
    }
    void setAutoScroll(bool b){
        m_autoScroll = b;
    }
    bool isAutoScroll(){
        return m_autoScroll;
    }
    void show(bool scroll){
        recalcRect();
        isFocused(true);
        setAutoScroll(scroll);
        needDraw(true);
        //m_select = -1;
        //m_firstrow = -1;
        draw();
    }
    void clear(){
        m_strings.clear();
        m_select = -1;
        m_firstrow = -1;
        update();
    }
    void update(){
        recalcRect();
        needDraw(true);
        draw();
    }
    void draw(){
        if (!needDraw())
            return;
        //drawFrame(m_top,COLORMID,COLORMID);
        GRect r = getClientRectInWindow();
        GPoint p(r.x1,r.y1);
        if (m_strings.size() > 0) {
        firstRow();
        m_showStringRect = showStringRect(m_strings,p,COLORWHITE,m_firstrow,m_firstrow+m_maxline);
        hightLightRow(m_select - m_firstrow);
        m_scrollBarVert.draw();
        }
        needDraw(false);
    }
    void firstRow(){
        GRect r = m_scrollBarVert.getWindowRectInParent();
        GRect pr = getWindowRectInParent();
        r.add(pr.x1,pr.y1);
        GRect c = getClientRectInParent();
        m_firstrow = ((r.y1 - c.y1)  * m_strings.size() + c.height() - 3 ) / c.height();
    }
    void addList(const GString& s){
        m_strings.push_back(s);
    }
    void recalcRect(){
        GRect r = getWindowRectInParent();
        m_top.x1 = 0;
        m_top.y1 = 0;
        m_top.x2 = r.x2;
        m_top.y2 = m_top_height;
        int border = getBorderWidth();
        GRect rr (r.x1 + border, r.y1 + m_top_height + border,
                r.x2 - border,r.y2 -border);
        setclientRectInparent(rr);
        int size = m_strings.size();
        int scroll_height;
        if (size > m_maxline){
            scroll_height = (m_maxline * m_string_height  * m_maxline + size -1 ) / size; 
        }else{
            scroll_height = m_string_height * size;
        }

        //rr.x1 = rr.x2 - 14 - border;
        rr.x1 = r.width() - 14 - border; ;
        rr.y1 = m_top_height + border;
        //rr.x2 = rr.x2 - border;
        rr.x2 = r.width() - border;
        rr.y2 = rr.y1 +  scroll_height;;

        m_scrollBarVert.setWindowRectInParent(rr);
        if ( size > m_maxline)
            m_scrollBarVert.setMoveHeight(m_string_height * m_maxline);
        else
            m_scrollBarVert.setMoveHeight(m_string_height * size);

        m_vertbar.x1 = r.x2 - m_top_height;
        m_vertbar.y1 = r.y1 - m_top_height;
        m_vertbar.x2 = r.x2;
        int height = r.height() / m_item_count;
        m_vertbar.y2 = m_vertbar.y1 + height;
        m_horibar.x1 = r.x1;
        m_horibar.y1 = r.y2 - m_top_height;
        int width = r.width() / m_item_length;
        m_horibar.x2 = r.x1 + width;
        m_horibar.y2 = r.y2;
    }
    MSG forChilds(const XEvent& ev){
        XEvent e = ev;
        for (auto a : getChilds()){
            if (a->processEvent(e) == MSG_NODRAW)
                return MSG_NODRAW;
        }
        return MSG_CONTINUE;
    }
    MSG onPointerMove(const XEvent& ev){
        showSelect(ev);
        return MSG_NODRAW;
    }
    // GListBox
    // ev have changed to containner's xy
    void showSelect(const XEvent& ev){
            int size = min<int>(m_maxline ,m_strings.size());
            int space = getFontStringHeight();
            GRect r = getWindowRectInParent();
            int y = yOfEv(ev); 
            int row = -1;
            y -= r.y1;
            if (y > 0) {
                row = y  / space;
                if  (row >= 0 && row  < size ){
                    showString();
                    m_scrollBarVert.draw();
                    m_select = row  + m_firstrow;
                    hightLightRow(row);
                    m_scrollBarVert.draw();
                }
            }
    }
    
    MSG onButtonDown(const XEvent& ev){
        if (isForMe(ev)){
            showSelect(ev);
            return MSG_SELECTED;
        }
        else{
            isFocused(false);
            return MSG_NOSELECTED;
        }
    }
    MSG processEvent(const XEvent& e){
        if (forChilds(e) == MSG_NODRAW){
            isFocused(true);
            return MSG_NODRAW;
        }
        if (m_scrollBarVert.processEvent(e) == MSG_NODRAW){
            isFocused(true);
            return MSG_NODRAW;
        }
        XEvent ev = xevevtToContainer(e);

        switch(ev.type){
            case Expose:
                recalcRect();
                draw();
                return MSG_CONTINUE;
        }
        if (isFocused()){
            if (isButtonDown(ev)){
                return onButtonDown(ev);
            } // 
            if (isForMe(ev) && isAutoScroll() && isPointerMove(ev)){
                return onPointerMove(ev);
            }
            return MSG_NODRAW;
        }
        if (isButtonDown(ev) && isForMe(ev)){
            return onButtonDown(ev);
        }
        return MSG_CONTINUE;
    }
    void hightLightRow(int row){
        int size = min<int>(m_strings.size(), m_maxline);
        int space = m_showStringRect.height() / size;
        if (row >= 0 && row < size){
            int y  = getClientRectInWindow().y1;
            int bx = m_showStringRect.x1;
            int ex = m_showStringRect.x2;
            GRect r (bx, row * space + y,
                    ex, row * space + space + y);
            showHighlightBar(r, COLORBLUE,m_strings[row + m_firstrow]);
        }
    }
    void showString(){
        GRect r = getClientRectInWindow();
        m_showStringRect = showStringRect(m_strings,GPoint(r.x1,r.y1), COLORWHITE,m_firstrow,m_firstrow + m_maxline);
    }

};

class GList  :public  GWindow<GList>
{
    private:
        bool m_btn_down = false;
        int m_select = 0;
        int row = 0;
        int oldrow = -1;
        GListBox * m_listbox = NULL;
    public:
        //constructor
        
        GList (){ 
            init();
        }
        GList(const GString& s){
            init();
            setTitle(s);
        }
        //GList (const GList & s);
        //GList & operator=(const GList & s);
        ~GList (){
            if (m_listbox != NULL)
                delete m_listbox;
        };
    public:
        //maniulator
        void init(){
            setWindowRectInParent(0,0,400,20);
            m_listbox = new GListBox();
            m_listbox->setParent(this);
        }
        void addList(const GString& s){
            m_listbox->addList(s);
        }
        void drawDownArrow(GRect& r){
            clearArrow(r);
            setForeground(COLORBLACK);
            int x = r.x2 - 10 - 5;
            int y = (r.height() - 5) / 2 ;
            int ex = r.x2 -5 ;
            for (int i = 0;i<6;i++){
                drawLine(x,y, ex, y);
                x++,y++,ex--;
            }
        }
        void drawLeftArrow(GRect& r){
            setForeground(COLORBLACK);
            int x = r.x2 - 5;
            int y = (r.height() - 5) / 2 ;
            int ey = y + 10;
            for (int i = 0;i<6;i++){
                drawLine(x,y, x, ey);
                x--,y++,ey--;
            }

        }
        void clearArrow(GRect& r){
            setForeground(COLORWHITE);
            fillRectangle(r.x2 -10, r.y1 + 5, r.x2 - 4, r.y2 - 5);
        }
        void draw(){
            if (!needDraw())
                return ;
            GRect r = getClientRectInWindow();
            drawSolid(r,COLORWHITE);
            drawFrame(r);
            drawLeftArrow(r);
            drawStringCenterY(r,getTitle());
            needDraw(false);
        }
        MSG onExpose(){
                m_btn_down = false;
                oldrow= -1;
                GRect r = getWindowRectInParent();
                int h = m_listbox->getDefaultStringHeight();
                m_listbox->setWindowRectInParent(0,r.height(),r.width(),r.height()+h);
                m_listbox->recalcRect();
                needDraw(true);
                draw();
                //return MSG_REDRAW;
                return MSG_CONTINUE;
         
        }
        void onButtonDown(){
                if (m_btn_down == true) {
                    m_btn_down = false;
                    needDraw(true);
                    draw();
                    //return MSG_REDRAW;
                }
                else{
                    m_btn_down = true;
                    GRect r = getClientRectInWindow();
                    drawDownArrow(r);
                    //return MSG_NODRAW;
                }
        }
        MSG onListBoxShow(const XEvent& e){
                switch (m_listbox->processEvent(e)){
                    case MSG_NOSELECTED:
                        onButtonDown();
                        return MSG_REDRAW;
                    case MSG_SELECTED:
                        onButtonDown();
                        setTitle(m_listbox->getSelected());
                        return MSG_REDRAW;
                }
                return MSG_NODRAW;
        }
        bool isListBoxShow(){
            return m_btn_down;
        }
        MSG processEvent(const XEvent& e){
            if (e.type == Expose){
                return onExpose();
            }
           // if button down no other window should redraw
            if (isListBoxShow()){
                return onListBoxShow(e);
            }
            XEvent ev = xevevtToContainer(e);
            if (isForMe(ev) && isButtonDown(ev)){
                m_btn_down = true;
                m_listbox->show(true);
                return MSG_NODRAW;
            }
            return MSG_CONTINUE;
        }
       //accessor
       int setSelected(int w){
           if (m_listbox != NULL){
               m_listbox->setSelected(w);
               setTitle(m_listbox->getSelected());
           }
           return -1;
       }
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
            GRect r = getWindowRectInParent();
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
            drawStringCenterY(r,getTitle());
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
        GWindowBase* m_targetWin = NULL;
    public:
        virtual void draw() {
            drawFrame();
            if(m_menu != NULL){
                m_menu->draw();
            }
            drawAllWindow();
        }
        void recalcRect(){
            GRect r = getWindowRectInParent();
            r.shrink(getBorderWidth());
            setclientRectInparent(r);
            updateStatus();
        }
        void innerdraw(){
            drawFrame();
        }
    public:
        GFrame():m_StatusBar("Status"){
            setTitle("Frame");
            m_StatusBar.setParent(this);
        }
        GFrame(const GString& s){
            setTitle(s);
            m_StatusBar.setParent(this);
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
                GRect cr = a->getWindowRectInParent();
                if (isCrossRect(r, cr)){
                    a->draw();
                }
            }
        }

        void updateStatus(){
            GRect rootrect = getClientRectInParent();
            rootrect.move(0,0);
            int height = m_StatusBar.getFontStringRect(getTitle()).height();
            m_StatusBar.setWindowRectInParent(rootrect.x1, rootrect.y2 - height -4, rootrect.x2, rootrect.y2 );
        }

        void drawStatus(const XEvent& ev){
            GString s;
            int x = getAbsolutePosition().x;
            int y = getAbsolutePosition().y;
            // to frame local xy;
            int xx = ev.xbutton.x - x;
            int yy = ev.xbutton.y - y;
            s.format("%d,%d,%d,%d %s",xx , yy, x,y, (const char*)getTitle());
            m_StatusBar.setTitle(s);
            m_StatusBar.draw();
        }
        bool isReserved(){
            GRect r = getWindowRectInParent();
            for (auto a : getChilds()){
                if (isCrossRect(r, a->getReservedRect())) {
                    return true;
                }
            }
            return false;
        }

       void drawFrame(){
            GRect r = getClientRectInWindow();
            if (isReserved())
                return;
            drawSolid(r,COLORMID);
            GWindow::drawFrame(r);
            //m_StatusBar.draw();
        }
        MSG processEvent(XEvent& ev){
            return MSG_CONTINUE;
        }
        MSG innerprocessEvent(const XEvent& ev){
            /*
               if (m_menu != NULL){
               if (m_menu->processEvent(ev) == MSG_REDRAW){
               draw();
               return MSG_REDRAW;
               }
               }
               */
            XEvent e = ev;
                   if (m_targetWin !=NULL){
                switch (m_targetWin -> processEvent(e)){
                    case MSG_NODRAW:
                        return MSG_NODRAW;
                    case MSG_REDRAW:
                        draw();
                        return MSG_REDRAW;
                    case MSG_CONTINUE:
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
            // Expose have many events
            //updateStatus();
            drawStatus(ev);
            return MSG_CONTINUE;
        }
};

class GLayout 
{
    protected:
        GFrame * m_frame;
        int m_sep = 2;
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
            GRect r = f->getClientRectInParent();
            f->updateStatus();
            if  ( ct == 0)
                return;
            GStatus * status  = f->getStatusBar();
            int h = (r.height() - status->getWindowRectInParent().height());
            int w = (r.width() - ct * m_sep) / ct;
            int x = 0;
            int y = 0;
            for (auto a : f->getChilds()){
                a->setWindowRectInParent(x,y,w + x,h); 
                a->recalcRect();
                a->layout();
                x += w + m_sep;
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
            GRect r = f->getClientRectInParent();
            f->updateStatus();
            if  ( w == 0)
                return;
            GStatus *status = f->getStatusBar();
            int h = (r.height() - status->getWindowRectInParent().height() - w * m_sep) / w;
            int x = 0;
            int y = 0;
            for (auto a : f->getChilds()){
                a->setWindowRectInParent(x,y,r.width(),y + h); 
                a->recalcRect();
                a->layout();
                y += h + m_sep;
            }
        }
};

class GFrameLayout : public GFrame
{
    private :
        GLayout * m_layout = NULL;
    public:
        void layout(){
            GFrame::recalcRect();
            m_layout->layout();
        }
        GFrameLayout(const GString& s) : GFrame(s){
            m_layout = new GLayoutVertical();
            m_layout->setFrame(this);
        };
        GFrameLayout(){
            m_layout = new GLayoutVertical();
            m_layout->setFrame(this);
        }
        ~GFrameLayout(){
            if (m_layout != NULL){
                delete m_layout;
            }
        }
        void addLayout(GLayout * l){
            // memory leak
            if (m_layout != NULL)
                delete m_layout;
            m_layout = l;
            l->setFrame(this);
        }
        //Expose is not first XEvent;
        MSG processEvent(const XEvent& e){
            if (e.type == Expose ){
                layout();
                //draw(); 
                innerdraw();
            }
            return innerprocessEvent(e);
        }
};
#endif

