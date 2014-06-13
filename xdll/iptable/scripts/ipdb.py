
import win32com.client
import datetime

import socket
import struct





def parse_ipdb(name, sqlText, funcParseRow) :
	conn = win32com.client.Dispatch("ADODB.Connection")
	dsn = 'PROVIDER=Microsoft.Jet.OLEDB.4.0;DATA SOURCE=%s.mdb;' % name
	conn.Open(dsn)
	#rst = win32com.client.Dispatch("ADODB.Recordset")
	#rst.Open("select * from ip", conn, 2, 1)
	rstResult = conn.Execute(sqlText)
	rst = rstResult[0]
	count = 0
	startTime = datetime.datetime.now()
	print startTime
	fout = file(name + "_base.txt", "w")
	while not rst.EOF :
		funcParseRow(rst.Fields, fout)
		rst.MoveNext()
		count += 1
		if count % 100 == 0 :
			print "=",
			#if count % 8000 == 0 :
			#	break
	print ""
	fout.close()
	endTime = datetime.datetime.now()
	print endTime
	print "total item count is", count
	print "used time is", endTime - startTime




def to_ascii(s) :
	if s == None :
		return ""
	if isinstance(s, type(u"1")) :
		return s.encode("gbk")
	return str(s)


def ip_to_string(ip) :
	return socket.inet_ntoa(struct.pack('L', ip))


def host_ip_to_string(ip) :
	return ip_to_string(socket.htonl(ip))



def parse_city_ip_row(fields, fout) :
	start_ip = long(fields.Item(0).Value)
	end_ip = long(fields.Item(1).Value)
	province = to_ascii(fields.Item(2).Value)
	city = to_ascii(fields.Item(3).Value)
	isp = to_ascii(fields.Item(4).Value)
	ipstr1 = host_ip_to_string(start_ip)
	ipstr2 = host_ip_to_string(end_ip)
	linestr = "%15s, %15s, 0x%08x, 0x%08x, %8s, %20s,  %s\n" % (ipstr1, ipstr2, start_ip, end_ip, province, city, isp)
	fout.write(linestr)

def parse_country_ip_row(fields, fout) :
	start_ip = long(fields.Item(0).Value)
	end_ip = long(fields.Item(1).Value)
	country = to_ascii(fields.Item(2).Value)
	ipstr1 = host_ip_to_string(start_ip)
	ipstr2 = host_ip_to_string(end_ip)
	linestr = "%15s, %15s, 0x%08x, 0x%08x, %s\n" % (ipstr1, ipstr2, start_ip, end_ip, country)
	fout.write(linestr)

def parse_country_code_row(fields, fout) :
	c2code = to_ascii(fields.Item(0).Value)
	c3code = to_ascii(fields.Item(1).Value)
	englishName = to_ascii(fields.Item(2).Value)
	chineseName = to_ascii(fields.Item(3).Value)
	area = to_ascii(fields.Item(4).Value)
	linestr = "%4s,%5s,%26s,%46s,   %s\n" % (c2code, c3code, chineseName, englishName, area)
	fout.write(linestr)


def parse_city_ip() :
	parse_ipdb("city_ip", "select * from ip order by start_ip", parse_city_ip_row)

def parse_country_ip() :
	parse_ipdb("country_ip", "select * from ip order by start_ip", parse_country_ip_row)

def parse_country_code() :
	parse_ipdb("code_country", "select * from code", parse_country_code_row)

def parse_all() :
	parse_city_ip()
	parse_country_ip()
	parse_country_code()




parsing_params = { 
	"city_ip": parse_city_ip, 
	"country_ip": parse_country_ip, 
	"code_country": parse_country_code, 
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

