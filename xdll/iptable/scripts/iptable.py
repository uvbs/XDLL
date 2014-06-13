
# -*- coding: gb2312

import msvcrt
import datetime
import socket
import struct


def ip_to_string(ip) :
    return socket.inet_ntoa(struct.pack('L', ip))


def host_ip_to_string(ip) :
    return ip_to_string(socket.htonl(ip))



class IPEntry :
    def __init__(self) :
        self.start_ip = 0L
        self.end_ip = 0L
        self.isp = ''
        self.province = ''
        self.city = ''

    def check_valid(self) :
        assert isinstance(self.start_ip, type(1L))
        assert isinstance(self.end_ip, type(1L))
        assert type(self.isp) == type('1')
        assert type(self.province) == type('1')
        assert type(self.city) == type('1')
        #if self.start_ip >= self.end_ip :
        #    print self.start_ip, self.end_ip, host_ip_to_string(self.start_ip), host_ip_to_string(self.end_ip)
        assert self.start_ip <= self.end_ip

    def __str__(self) :
        ipstr1 = host_ip_to_string(self.start_ip)
        ipstr2 = host_ip_to_string(self.end_ip)
        return "%15s, %15s,  0x%08x, 0x%08x,  %8s, %20s,  %s" % (ipstr1, ipstr2, self.start_ip, self.end_ip, self.province, self.city, self.isp)

    def __repr__(self) :
        return self.__str__()

    def contains(self, target) :
        return self.start_ip <= target.start_ip and self.end_ip >= target_ip


class CountryIPEntry :
    def __init__(self) :
        self.start_ip = 0L
        self.end_ip = 0L
        self.country = ''
        self.check_valid()

    def check_valid(self) :
        assert isinstance(self.start_ip, type(1L))
        assert isinstance(self.end_ip, type(1L))
        assert type(self.country) == type('1')
        assert self.start_ip <= self.end_ip

    def __str__(self) :
        ipstr1 = host_ip_to_string(self.start_ip)
        ipstr2 = host_ip_to_string(self.end_ip)
        return "%15s, %15s,  0x%08x, 0x%08x,  %s" % (ipstr1, ipstr2, self.start_ip, self.end_ip, self.country)

    def __repr__(self) :
        return self.__str__()


predefined_isps = [ 
"", "教育网", "电信", "网通", "联通", "铁通", "移通", "有线通", 
"长城宽带", "盈联宽带", "视讯宽带", "首都在线", "赛尔", 
"世纪互联" 
]

edu_isp_tag = [ 
"学校", "大学", "中学", "小学", "幼儿园", "学院", "农校", 
"一中", "二中", "三中", "四中", "五中", "六中", "七中", "八中", "九中", "十中", 
"电大", "学术", "学科", 
"中科院", "教育", "招生", "研究所", "教委", "科委", "少年宫", "科技网" 
]

def get_isp_name(name) :
    if is_edu_isp(name) :
        return "教育网"
    for isp in predefined_isps :
        if len(isp) > 0 and name.find(isp) != -1 :
            return isp
    return name


def is_edu_isp(name) :
    for tag in edu_isp_tag :
        if name.find(tag) != -1 :
            return True
    return False


class IndexItemInfo :
    def __init__(self, name, number) :
        self.name = name
        self.number = number
        self.count = 0



class Indexer :
    def __init__(self, itemType, predefinedItems = ['']) :
        self.item_type = itemType
        self.items = {}
        self.sorted_items = []
        self.number = 0
        for name in predefinedItems :
            self.index(name)

    def index(self, name) :
        if name in self.items :
            item = self.items[name]
        else :
            item = self.item_type(name, self.number)
            self.items[name] = item
            self.sorted_items.append(item)
            self.number += 1
        item.count += 1
        return item

    def get(self, name) :
        return self.items[name]


class ProvinceInfo(IndexItemInfo) :
    def __init__(self, name, number) :
        IndexItemInfo.__init__(self, name, number)
        self.cities = Indexer(IndexItemInfo)



class ISPIndexer(Indexer) :
    def __init__(self) :
        Indexer.__init__(self, IndexItemInfo, predefined_isps)

    def index(self, name) :
        ispName = get_isp_name(name)
        return Indexer.index(self, ispName)

    def get(self, name) :
        return self.items[get_isp_name(name)]


cpp_file_header = '''
#include "stdafx.h"

'''

cpp_if_debug_begin = '''

#ifdef _DEBUG

'''

cpp_if_debug_end = '''

#endif

'''

ip_entry_definition = '''

/*
/// ip表中的一项
struct IP_INFO_ENTRY
{
	/// 起始ip
	UINT32 StartIP;

	/// 结束ip
	UINT32 EndIP;

	/// 所属isp，或者是教育网的某个地址
	WORD ISP;

	/// 所在省
	WORD Province;

	/// 所在城市
	WORD City;
};
*/

/// ip数据表
const CITY_IP_INFO CITY_IP_LIST[] = 
{
'''




def parse_iptable(parser) :
    count = 0
    startTime = datetime.datetime.now()
    print startTime
    fin = file(parser.name + "_base.txt", "r")
    for line in fin :
        items = line.rstrip('\n').split(',')
        items = [ x.strip() for x in items]
        parser.parse(items)
        count += 1
        if count % 100 == 0 :
            print "=",
            #if count % 8000 == 0 :
            #    break
    print ""
    endTime = datetime.datetime.now()
    parser.output()
    parser.generate()
    print endTime
    print "total item count is", count
    print "used time is", endTime - startTime



class iptable_parser :
    def __init__(self, name) :
        self.name = name
        self.entries = []

    def output(self) :
        lastOverlappedItems = []
        fout = file(self.name + '_base2.txt', 'w')
        fout2 = file(self.name + '_special.txt', 'w')
        ignoredCount = 0
        lastItem = None
        for item in self.entries :
            assert len(lastOverlappedItems) <= 2
            if len(lastOverlappedItems) == 0 :
                lastOverlappedItems = [item]
                continue
            if item.start_ip == lastOverlappedItems[0].start_ip :
                lastOverlappedItems.append(item)
                continue
            if len(lastOverlappedItems) == 1 :
                fout.write("%s\n" % lastOverlappedItems[0])
                lastOverlappedItems = [item]
                continue
            nextStartIP = item.start_ip
            nextEndIP = item.end_ip
            for overlappedItem in lastOverlappedItems :
                assert overlappedItem.end_ip < nextStartIP or overlappedItem.end_ip >= nextEndIP
            assert len(lastOverlappedItems) == 2
            lastItem1 = lastOverlappedItems[0]
            lastItem2 = lastOverlappedItems[1]
        if lastItem is not None :
            fout.write("%s\n" % lastItem)
        fout.close()
        fout2.close()
        print "Ignore", ignoredCount, "items."



class country_ip_parser(iptable_parser) :
    def __init__(self) :
        iptable_parser.__init__(self, "country_ip")
        self.countries = Indexer(IndexItemInfo)
        self.last_start_ip = 0

    def parse(self, items) :
        assert len(items) == 5
        entry = CountryIPEntry()
        start_ip_str = items[0]
        end_ip_str = items[1]
        entry.start_ip = long(items[2], 16)
        entry.end_ip = long(items[3], 16)
        assert host_ip_to_string(entry.start_ip) == start_ip_str
        assert host_ip_to_string(entry.end_ip) == end_ip_str
        entry.country = items[4]
        entry.check_valid()
        self.entries.append(entry)
        assert entry.start_ip >= self.last_start_ip
        self.countries.index(entry.country)

    def generate(self) :
        fout = file("country_ip.cpp", "w")
        fout.write("const char* COUNTRY_ARRAY[] = \n{\n")
        for country in self.countries.sorted_items :
            fout.write('\t"%s", // %d\n' % (country.name, country.number))
        fout.write("};\n\n\n")
        for ipItem in self.entries :
            country = self.countries.get(ipItem.country)
            ipstr1 = host_ip_to_string(ipItem.start_ip)
            ipstr2 = host_ip_to_string(ipItem.end_ip)
            line = '\t{ 0x%08x, 0x%08x, %d }, \t\t// %s - %s\n' % (ipItem.start_ip, ipItem.end_ip, country.number, ipstr1, ipstr2)
            fout.write(line)
        fout.write("};\n\n\n\n")
        fout.close()



class city_ip_parser(iptable_parser) :
    def __init__(self) :
        iptable_parser.__init__(self, "city_ip")
        self.isps = ISPIndexer()
        self.provinces = Indexer(ProvinceInfo)
        self.last_start_ip = 0

    def parse(self, items) :
        assert len(items) == 7
        entry = IPEntry()
        start_ip_str = items[0]
        end_ip_str = items[1]
        entry.start_ip = long(items[2], 16)
        entry.end_ip = long(items[3], 16)
        assert host_ip_to_string(entry.start_ip) == start_ip_str
        assert host_ip_to_string(entry.end_ip) == end_ip_str
        entry.province = items[4]
        entry.city = items[5]
        entry.isp = items[6]
        entry.check_valid()
        self.entries.append(entry)
        self.isps.index(entry.isp)
        province = self.provinces.index(entry.province)
        province.cities.index(entry.city)
        assert entry.start_ip >= self.last_start_ip
        self.last_start_ip = entry.start_ip

    def generate(self):
        self.generate0()
        self.generate_text()
        self.generate_dbtext()

    def generate0(self) :
        fout = file("isplist.cpp", "w")
        fout.write(cpp_file_header)
        fout.write(cpp_if_debug_begin)
        fout.write("const char* ISP_ARRAY[] = \n{\n")
        for isp in self.isps.sorted_items :
            #fout.write('\t"%s", // %d\n' % (isp.name, isp.number))
            fout.write('\t"%s", \n' % (isp.name))
        fout.write("};\n\n\n")
        fout.write("const char* PROVINCE_ARRAY[] = \n{\n")
        for province in self.provinces.sorted_items :
            fout.write('\t"%s", \n' % province.name)
            for city in province.cities.sorted_items :
                line = "  %d,  %d,  %s\n" % (city.number, city.count, city.name,)
                #fout.write(line)
            #fout.write("\n\n")
        fout.write("};\n\n\n")
        fout.write(cpp_if_debug_end)
        fout.write(ip_entry_definition)
        for ipItem in self.entries :
            isp = self.isps.get(ipItem.isp)
            province = self.provinces.get(ipItem.province)
            city = province.cities.get(ipItem.city)
            if city.number >= 100 :
                print isp.name, province.name, city.name, city.number
            ipstr1 = host_ip_to_string(ipItem.start_ip)
            ipstr2 = host_ip_to_string(ipItem.end_ip)
            line = '\t{ 0x%08x, 0x%08x, %d, %d, %d }, \t\t// %s - %s\n' % (ipItem.start_ip, ipItem.end_ip, isp.number, province.number, city.number, ipstr1, ipstr2)
            fout.write(line)
        fout.write("};\n\n\n\n")
        fout.close()

    def generate_text(self):
        fout = file('regions.txt', 'w')
        fout.write('\n\n')
        fout.write('isp list:\n')
        for isp in self.isps.sorted_items :
            fout.write('%-4d  %s\n' % (isp.number, isp.name))
        fout.write('\n' * 4)
        fout.write('province list:\n')
        for province in self.provinces.sorted_items:
            fout.write('%-4d  %s\n' % (province.number, province.name))
        fout.write('\n' * 4)
        fout.write('city list:\n')
        for province in self.provinces.sorted_items:
            fout.write('%-4d  %s\n' % (province.number, province.name))
            for city in province.cities.sorted_items:
                fout.write('%-4d  %s\n' % (city.number, city.name))
            fout.write('\n' * 1)
        fout.write('\n' * 2)
        fout.close()

    def generate_dbtext(self):
        fout = file('regiondb.txt', 'w')
        for province in self.provinces.sorted_items:
            fout.write('16\t%d\t0\t%s\n' % (province.number, province.name))
        fout.write('\n' * 1)
        for province in self.provinces.sorted_items:
            for city in province.cities.sorted_items:
                if city.number > 0:
                    fout.write('16\t%d\t%d\t%s\n' % (province.number, city.number, city.name))
        fout.write('\n' * 2)
        fout.close()

def parse_city_ip() :
    parse_iptable(city_ip_parser())

def parse_country_ip() :
    parse_iptable(country_ip_parser())

def parse_country_code() :
    parse_iptable(code_country_parser())

def parse_all() :
    parse_city_ip()
    parse_country_ip()
    #parse_country_code()


parsing_params = { 
    "city_ip": parse_city_ip, 
    "country_ip": parse_country_ip, 
    #"code_country": parse_country_code, 
    "all": parse_all
}


def run_main() :
    import sys
    if len(sys.argv) < 2 :
        print "no parameter is specified."
        return
    name = sys.argv[1]
    if name not in parsing_params :
        print "invalid parameter."
        return
    fun = parsing_params[name]
    fun()



if __name__ == "__main__" :
    run_main()

