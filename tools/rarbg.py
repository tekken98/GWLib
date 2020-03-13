#!/usr/bin/env python
import urllib2
import ssl
import gzip
import re
import sys
import os
import getopt
def readCookie():
    n = '/home/bao/bin/cookie.txt'
    file = open(n,'r')
    cont = file.read()
    cont = cont.replace('"','')
    l = cont.split('\n')
    s=""
    for a in l:
        if a != "" and a[0] !="#":
            s += a + ';'
    print s
    header['cookie']=s
    
def setSearchString(s ,page,category):
    s = s.replace(" ","%20")
    url = host + '/torrents.php?category='+category + '&search='+s+'&order=seeders&by=DESC&page='+str(page)
    #url = host + '/torrents.php?search='+s+'&order=seeders&by=DESC&page='+str(page)
    print url
    return url
def getZipHtml(url,header):
    request = urllib2.Request(url=url,headers=header)
    try: 
        r = urllib2.urlopen(request,timeout=timeout)
    except urllib2.URLError,e:
        print e.reason
        return None
    try:
        data = r.read()
        f=open("/home/bao/Downloads/a.gz","w")
        f.write(data)
        f.close()
        #print "get data:" + data
        #print (r.code)
        file = gzip.GzipFile('/home/bao/Downloads/a.gz','r')
        htm = file.read()
        file.close()
        #os.remove('a.gz')
        return htm
    except:
        return None
def getHtmlPlain(url,header):
    request = urllib2.Request(url=url,headers=header)
    try:
        r = urllib2.urlopen(request,timeout=timeout)
        data = r.read()
        return data
    except:
        return None

def getlinks(html):
    res = re.findall('<tr class="lista2">.*?</tr>',html,re.S)
    lk=[]
    for tr in res:
        oneres = re.findall('<td.*?</td>',tr,re.S)
        #print oneres[1]
        link = re.search('href="(.*?)".*?>(.*?)<',oneres[1],re.S)
        #print link.group(1)
        lk.append(link.group(1))
        #print link.group(2)
    return lk
#file = codecs.open('out.htm','r','utf-8')
def getContent(file):
    file = open(file,'r')
    content = file.read()
    file.close()
    #print (getlinks(content))
    return content

def checkExist(name):
    name = name.replace('%20',' ')
    dst = '/home/bao/.torrent/downlist.txt'
    try:
        file = open(dst, 'r')
    except:
        try:
            os.mkdir('/home/bao/.torrent/')
        except:
            None
        file = open(dst,'w')
        file.write(name)
        file.close()
        return False
    content = file.read()
    file.close()
    r = content.find(name)
    if (r != -1):
        print  name + " alread have downloaded!"
        return True
    else:
        try:
            file = open(dst,'w')
            file.write(content + name + "\n")
            file.close()
        except:
            print "error write " + dst
        return False
#html is torrent page
def downloadMagnet(html):
    savepath='/home/bao/Downloads/'
    res = re.search('href="(magnet:.*?)">',html,re.S)
    file = open(savepath + "magnet.txt","a+")
    file.write (res.group(1) + "\n")
    file.close()
    
def downloadTorrent(html):
    savepath='/home/bao/Downloads/'
    res = re.search('"(/download.php?.*?torrent)"',html,re.S)
    #print res.group(1)
    torrent = res.group(1)
    name = torrent.split('f=')
    savename = name[-1]
    if checkExist(savename) :
        return None
    try:
        w = open(savepath + savename)
        print savename + " exists!"
    except:
        downloadMagnet(html)
        return
        url = host + torrent
        torrentFile = getHtmlPlain(url,header)
        if (torrentFile != None):
            print url
            try:
                file = open(savepath+ savename,'w')
            except:
                file = open(savepath + savename[:64] + savename[-8:],'w')
            file.write(torrentFile)
            file.close()

# all links
def downloadAllTorrent(links):
    for a in links:
        print "get " +  a 
        torrentPage = getZipHtml(host + a,header)
        if (torrentPage != None) : 
            print "got"
            downloadTorrent(torrentPage)

def run(searchStr, pa,category):
    #url2 = 'https://rarbgaccess.org/torrents.php?category=2;4&search='+ searchStr+'&order=seeders&by=DESC'
    url = setSearchString(searchStr,pa,category)
    readCookie()
    
    content = getZipHtml(url,header)
    if content == None:
        print "get " + url + " empty"
        quit()
    #content = getContent('a.htm')
    links = getlinks(content)
    downloadAllTorrent(links)
    #print host+links[0]
    #torrentPage = getHtml(host + links[0],header)
    #file = open('page.html','w')
    #file.write(torrentPage)
    #file.close()
    #print torrentPage

def test():
    file = gzip.GzipFile('/home/bao/Downloads/a.gz','r')
    htm = file.read()
    file.close()
    downloadMagnet(htm)

def help():
    print 
    print '   rarbg.py -t 30 -p 1 "search string"'
    print """
             -t --timeout  connect timeout
             -c --category category
             -p --page     pagenumber
             -h --help     this
             """
def helpcategory():
    print "Category is :"
    for k in category_dict.keys():
        print "\t" + k

def getcategory(s):
    global category_dict
    for (key,value) in category_dict.items():
        if s == key :
            return value
    return ""

def getoptions():
    global timeout
    global pagenum
    global searchstr
    global category
    opts,args=getopt.getopt(sys.argv[1:],"hc:t:p:",["help","category=","timeout=","page="])
    for o,a in opts:
        if o in ("-t","--timeout"):
            timeout = int(a)
        if o in ("-c","--category"):
            if a == "help":
                helpcategory()
                sys.exit()
            category = getcategory(a)
        if o in ("-p","--page"):
            pagenum = int(a)
        if o in ("-h","--help"):
            help()
            sys.exit()
    if len(args) == 0:
        help()
        sys.exit()
    searchstr = args

# program begin     
ssl._create_default_https_context = ssl._create_unverified_context
context = ssl._create_unverified_context
host = 'http://rarbgmirror.org'
host = 'http://rarbgaccess.org'
searchstr=""
timeout=30
pagenum=1
category="4"
category_dict = {
        "Movie":"movies",
        "XXX":"2;4",
        "TV Shows":"2;18;41;49",
        "Games":"2;27;28;29;30;31;32;40;54",
        "Music":"2;23;24;25;26",
        "Software":"2;33;34;43",
        "None XXX":"2;14;15;16;17;21;22;42;18;19;41;27;28;29;30;31;32;40;23;24;25;26;33;34;43;44;45;46;47;48;49;50;51;52",
        }

header={
        'accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9',
        'accept-encoding': 'gzip',
        'accept-language': 'en-US,en;q=0.9',
#        'referer': 'https://rarbgaccess.org/index80.php',
        'sec-fetch-mode': 'navigate',
        'sec-fetch-site': 'same-origin',
        'sec-fetch-user': '?1',
        'upgrade-insecure-requests': '1',
        'user-agent': 'Mozilla/5.0 (X11; Linux i686) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.130 Safari/537.36',
        }
getoptions()
print searchstr, timeout , pagenum,category
run(searchstr[0],pagenum,category) 
#readCookie()
