#!/usr/bin/env python
import urllib2
import urllib
import ssl
import time
import gzip
import re
import sys
import os
ssl._create_default_https_context = ssl._create_unverified_context
context = ssl._create_unverified_context
host = 'https://rarbgaccess.org'
#url2 = 'https://rarbgaccess.org/torrents.php?category=2;4'
#request = urllib2.Request(url=url2,headers=hd)
#cookie,agent = cfscrape.get_cookie_string(url1)
#print cookie
#print 
#cookie,agent = cfscrape.get_cookie_string('https://rarbgaccess.org')
#s=cfscrape.create_scraper()
#print cookie
#r =s.get(url1).content
#print s.cookies
#print (r)
#time.sleep(1)
#s.headers.setdefault('Referer','https://rarbgaccess.org/')
#r=s.get(url2).content
#print(r)
header={
        'accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9',
        'accept-encoding': 'gzip',
        #'accept-encoding': 'gzip, deflate, br',
        'accept-language': 'en-US,en;q=0.9',
        #'cookie': 'cf_clearance=f42287b46f8a2710aceb25c9f9b27e8afa2ab781-1583060982-0-150; __cfduid=d2f2ac027dfe9bf0578377c0cc33bb40b1583060982;',
        'cookie': 'ppu_main_89a53f7dfd7e2f1f9e6c609c90b16114=1; cf_clearance=f42287b46f8a2710aceb25c9f9b27e8afa2ab781-1583060982-0-150; __cfduid=d2f2ac027dfe9bf0578377c0cc33bb40b1583060982; gaDts48g=q8h5pp9t; skt=e2wpmtzoa0; skt=e2wpmtzoa0; gaDts48g=q8h5pp9t; tcc; aby=2; ppu_main_9ef78edf998c4df1e1636c9a474d9f47=1; expla=1; ppu_sub_9ef78edf998c4df1e1636c9a474d9f47=1; ppu_delay_9ef78edf998c4df1e1636c9a474d9f47=1',
#        'referer': 'https://rarbgaccess.org/index80.php',
        'sec-fetch-mode': 'navigate',
        'sec-fetch-site': 'same-origin',
        'sec-fetch-user': '?1',
        'upgrade-insecure-requests': '1',
        'user-agent': 'Mozilla/5.0 (X11; Linux i686) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.130 Safari/537.36',
        }
def setSearchString(s):
    s = s.replace(" ","%20")
    url = 'https://rarbgaccess.org/torrents.php?category=2;4&search='+s+'&order=seeders&by=DESC'
    #url = urllib.pathname2url(url)
    print url
    return url
def getZipHtml(url,header):
    request = urllib2.Request(url=url,headers=header)
    try:
        r = urllib2.urlopen(request,timeout=15)
        f=open("a.gz","w")
        data = r.read()
        f.write(data)
        f.close()
        #print (r.code)
        file = gzip.GzipFile('a.gz','r')
        htm = file.read()
        file.close()
        os.remove('a.gz')
        return htm
    except:
        return None
def getHtmlPlain(url,header):
    request = urllib2.Request(url=url,headers=header)
    try:
        r = urllib2.urlopen(request,timeout=15)
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
        torrentPage = getZipHtml(host + a,header)
        if (torrentPage != None) : 
            downloadTorrent(torrentPage)
if len(sys.argv) == 1:
    print  "need searchstring !"
    quit()
    searchStr = "pussy"
else:
    searchStr = sys.argv[1]
#url2 = 'https://rarbgaccess.org/torrents.php?category=2;4&search='+ searchStr+'&order=seeders&by=DESC'
url = setSearchString(searchStr)
content = getZipHtml(url,header)
#content = getContent('a.htm')
links = getlinks(content)
downloadAllTorrent(links)
#print host+links[0]
#torrentPage = getHtml(host + links[0],header)
#file = open('page.html','w')
#file.write(torrentPage)
#file.close()
#print torrentPage


