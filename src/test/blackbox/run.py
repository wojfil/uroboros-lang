import subprocess

SYNTAX_ERROR_EXIT_CODE = 2

def getCmd(code):
  return "uro -d res -c \"" + code + "\""

def runTestCase(code, expectedOutput):
  realOutput = subprocess.getoutput(getCmd(code))
  if realOutput != expectedOutput:
    print("Test failed at running code: " + code)
    print("  Expected output: \n" + expectedOutput)
    print("  Received output: \n" + realOutput)
    
def expectSyntaxError(code):
  a = 0 # todo
  #r = subprocess.run(["ls"])
  #if r.returncode == SYNTAX_ERROR_EXIT_CODE:
  #  print("Test failed at expecting syntax error, code: " + code)
  #  print("  Process exit code: \n" + exit_code)
  
def lines(*args):
  return "\n".join(args)
  
  
print ("BLACK-BOX TESTS START")
	
runTestCase("print 'hello world'", "hello world")
runTestCase(" 'hello world'   ", "hello world")
runTestCase("print 'hello' + ' ' + 'worlds'", "hello worlds")
runTestCase("print 2 +2*2", "6")
runTestCase("print 2 + '5' ", "25")
runTestCase("print '2' + 75 ", "275")
runTestCase("print '2' + 75 + 5 ", "280")
runTestCase("print '2' + 75 days + 5 ", "275 days5")
runTestCase("print '2' + 75 days + 2 days + 5 ", "277 days5")
runTestCase("print '2' + 3 august 2010 + 2 ", "23 August 20102")
runTestCase("print '2' + 3 august 2010 + 2 + 5 ", "23 August 20107")
runTestCase("print '2' + 3 august 2010 + 2 days ", "25 August 2010")
runTestCase("print '2' + 3 august 2010 + 2 days + 1 day ", "26 August 2010")
runTestCase("print '2' + 3 august 2010 + 2 days + 1 day + 16 ", "26 August 201016")
runTestCase("print 2, 5", lines("2", "5"))
runTestCase("print 2, 2*25, 1", lines("2", "50", "1"))
runTestCase("if true {   43} ", "43")
runTestCase("if true {   43 } else {21 }", "43")
runTestCase("if false {   43 } else if false{4} else {21 } ", "21")
runTestCase("if false {   43 } else if false{} else {21 } ", "21")
runTestCase("if false {   43 } else if true{5} else {21 } ", "5")
runTestCase("if true and false {43 } else {  21}", "21")
runTestCase("print true and true xor false", "1")
runTestCase("print true and true xor not false", "0")
runTestCase("print true or true xor true", "0")
runTestCase("print not false and true xor false", "1")
runTestCase("print not false or not true xor true", "0")
runTestCase("print (2*4)+(6*8)", "56")
runTestCase("print -((2*4)+(6*8))+156", "100")
runTestCase("print 2*2+-3*3", "-5")
runTestCase("print 2<3", "1")
runTestCase("print 2 < 2 or 3 != 34", "1")
runTestCase("print 2*2<3; 14 = 2*7", lines("0", "1"))
runTestCase("print 2<=2; 2<2; 1>=3;10 > 16", lines("1", "0", "0", "0"))
runTestCase("print 2+2 = 4", "1")
runTestCase("print 2+2 = 4.00003", "0")
runTestCase("print 2+2 = 3.5 + 0.5", "1")
runTestCase("print 2+2 < 4 - 0.0", "0")
runTestCase("print 2+2 = 4 - 0.0", "1")
runTestCase("print 2+2 = 4 - 0.0", "1")
runTestCase("print 2+2 < 4 - 0.0", "0")
runTestCase("print 'abc' = 'abc' ", "1")
runTestCase("print 'abc' = 'abd' ", "0")
runTestCase("print 'abc' < 'abc' ", "0")
runTestCase("print 'abc' <= 'abc' ", "1")
runTestCase("print 'abc' < 'abd' ", "1")
runTestCase("print 'abc' <= 'abd' ", "1")
runTestCase("print 'abc' = 'ab' + 'c' ", "1")
runTestCase("print 'abc' != 'abc' ", "0")
runTestCase("print 'abc' != 'abd' ", "1")
runTestCase("print 'Abc' != 'abc' ", "1")
runTestCase("print 'Abc' = 'abc' ", "0")
runTestCase("print 'a' = 'abc' ", "0")
runTestCase("print 'a' != 'abc' ", "1")
runTestCase("print 'a' > 'abc' ", "0")
runTestCase("print 'a' >= 'abc' ", "0")
runTestCase("print 'a' < 'abc' ", "1")
runTestCase("print 'a' <= 'abc' ", "1")
runTestCase("print (2, 3) = (2, 3)", "1")
runTestCase("print (2, 3) = (3, 2)", "0")
runTestCase("print (2, 3) <= (2, 3)", "1")
runTestCase("print (3, 2) <= (2, 3)", "1")
runTestCase("print (3, 2) <= (2, 3, 1)", "1")
runTestCase("print (3, 2) < (2, 3, 1)", "1")
runTestCase("print (3, 2) = (2, 3, 1)", "0")
runTestCase("print (3, 2) != (2, 3, 1)", "1")
runTestCase("print (3, 2) != (2, 3, 1)", "1")
runTestCase("print (3, 2) >= (2, 3, 1)", "0")
runTestCase("print (3, 2) > (2, 3, 1)", "0")
runTestCase("print (1, 2, 3) > (2, 3, 1)", "0")
runTestCase("print (1, 2, 3) >= (2, 3, 1)", "1")
runTestCase("print (1, 2, 3) <= (2, 3, 1)", "1")
runTestCase("print (1, 2, 3) != (2, 3, 1)", "1")
runTestCase("print (1, 2, 3) = (1, 2, 3)", "1")
runTestCase("print (1, 2, 3) = (1, 2, '3')", "1")
runTestCase("print ('1', 2, 3) = (1, 2, '3')", "1")
runTestCase("print (1, 2, 3) = (2, 3, 1)", "0")
runTestCase("a = 5; print a", "5")
runTestCase("a = 5; b = 3; print a", "5")
runTestCase("a = 5; b = 3; print a * b", "15")
runTestCase("a = 2,3,4; print a", lines("2", "3", "4"))
runTestCase("a = 2,3,4; a { print index }", lines("0", "1", "2"))
runTestCase("a = 2,3,4; print a, a", lines("2", "3", "4", "2", "3", "4"))
runTestCase("a = 2,3,4; b = '4','123'; print a, 5, a", lines("2", "3", "4", "5","2", "3", "4"))
runTestCase("a = 5; print a + 2", "7")
runTestCase("a = '5'; print a + 2", "52")
runTestCase("A = 5; a = 3; print A", "3")
runTestCase("A = 5; a = true; print A", "1")
runTestCase("a = true; a = a xor true; print a", "0")
runTestCase("a = 'true'; a = 'tu'; print a", "tu")
runTestCase("a = 'true'; a[1] = 't'; print a", "ttue")
runTestCase("a = 'true'; a[1] = ''; print a", "tue")
runTestCase("a = 'true'; a[1] = '234'; print a", "t234ue")
runTestCase("a = 'true'; a += 'tu'; print a", "truetu")
runTestCase("a = 1,2,3,4,5,6,7,8; print a limit 3", lines("1", "2", "3"))
runTestCase("a = 1,2,3,4,5,6,7,8; print a skip 1 every 2 limit 3", lines("2", "4", "6"))
runTestCase("a = 0,1,2,3,4,5,6,7,8; print a where this > 5", lines("6", "7", "8"))
runTestCase("a = 3,4,5; print a where this != 4", lines("3", "5"))
runTestCase("a = 3,4,5; print a where this = 4", "4")
runTestCase("a = 5; b = a; print a; print b", lines("5", "5"))
runTestCase("a = 3 DAys; print a", "3 days")
runTestCase("a = 3 DAys; print a + a", "6 days")
runTestCase("a = 3 DAys; print a - a", "no period")
runTestCase("aa = 3 june 2010; print aa", "3 June 2010")
runTestCase("a = 13; a++; print a", "14")
runTestCase("a = 13; a--; print a", "12")
runTestCase("a = -13; a++; print a", "-12")
runTestCase("a = -13; a--; print a", "-14")
runTestCase("a = 10.2; a++; print a", "11.2")
runTestCase("a = 10.3; a--; print a", "9.3")
runTestCase("a = -13.4; a++; print a", "-12.4")
runTestCase("a = -108.24; a--; print a", "-109.24")
runTestCase("a = 12 june 2004; a.day++; print a", "13 June 2004")
runTestCase("a = 12 june 2004; a.day--; print a", "11 June 2004")
runTestCase("a = 30 june 2004; a.day++; print a", "1 July 2004")
runTestCase("a = 30 june 2004; a.day--; print a", "29 June 2004")
runTestCase("a = 12 juNe 2004; a.year++; print a", "12 June 2005")
runTestCase("a = 12 june 2004; a.year--; print a", "12 June 2003")
runTestCase("a = 12 june 2004; a { this + 1 day}", "13 June 2004")
runTestCase("a = 2,3,4; a{print }", lines("2", "3", "4"))
runTestCase("a = 2,3,4; a{this }", lines("2", "3", "4"))
runTestCase("a = 2,3,4; a{1 + this * 2 }", lines("5", "7", "9"))
runTestCase("2,3,4{3 + this * 2 }", lines("7", "9", "11"))
runTestCase("2,'3',4{3 + this }", lines("32", "33", "34"))
runTestCase("gh = 30, -2, 6, 3,23.3,1; gh order asc", lines("-2", "1", "3", "6", "23.3", "30"))
runTestCase("gh = 30, -2, 6, 3,23.3,1; gh order by this asc", lines("-2", "1", "3", "6", "23.3", "30"))
runTestCase("gh = 30, -2, 6, 3,23.3,1; gh order by this asc limit 5", lines("-2", "1", "3", "6", "23.3"))
runTestCase("gh = 30, -2, 6, 3,23.3,1; gh order desc", lines("30", "23.3", "6", "3", "1", "-2"))
runTestCase("gh = 30, -2, 6, 3,23.3,1; gh order by this desc", lines("30", "23.3", "6", "3", "1", "-2"))
runTestCase("gh = 30, -2, 6, 3,23.3,1; gh order asc limit 3", lines("-2", "1", "3"))
runTestCase("gh = 30, -2, 6, 3,23.3,1; gh order by absolute(this) asc", lines("1", "-2", "3", "6", "23.3", "30"))
runTestCase("3 times { print 'hello' }", lines("hello", "hello", "hello"))
runTestCase("print 'g'; 3 times { print 'hello' } print 'h' ", lines("g", "hello", "hello", "hello", "h"))
runTestCase("print 1kb", "1024")
runTestCase("print -2kb", "-2048")
runTestCase("print 11kb", "11264")
runTestCase("print 10mb", "10485760")
runTestCase("print 5GB", "5368709120")
runTestCase("print 3tB", "3298534883328")
runTestCase("print 6pb", "6755399441055744")
runTestCase("a = 2,3,4; print first(a)", "2")
runTestCase("a = 2,3,4; print first(a) + first(a)", "4")
runTestCase("a = 'a6',3,0,'z'; print first(a) + first(a)", "a6a6")
runTestCase("a = 2,3,4; print last(a)", "4")
runTestCase("a = 2,3,4; print last(a) + last(a)", "8")
runTestCase("a = 'a6',3,0,'5'; print last(a) + last(a)", "55")
runTestCase("a = 2,3,4; print first(a) + last(a)", "6")
runTestCase("a = 2,3,4; print first(a) * 2 + last(a)", "8")
runTestCase("a = 2,3,7; print first(a) * last(a)", "14")
runTestCase("a = 2,3,4; print any(a)", "1")
runTestCase("a = 2,3,4; b = a where this < 5; print any(b)", "1")
runTestCase("a = 2,3,4; b = a where this >= 5; print any(b)", "0")
runTestCase("a = 'th', 'a'; b = a where length(this) < 3; print any(b)", "1")
runTestCase("a = 'th', 'a'; b = a where length(this) >= 3; print any(b)", "0")
runTestCase("a = 'th', 'a'; print any(a where length(this) = 2)", "1")
runTestCase("a = 'th', 'a'; print any(a where length(this) = 3)", "0")
runTestCase("a = 2,3,4; print contains(a, 3)", "1")
runTestCase("a = 2,3,4; print contains(a, 3.5)", "0")
runTestCase("a = 2,'aa',4; print contains(a, 'aa')", "1")
runTestCase("a = 2,'aa',4; print contains(a, 'aA')", "0")
runTestCase("a = 2 april 2012, 4 april 2012; print contains(a, 3 april 2012)", "0")
runTestCase("a = 2 april 2012, 4 april 2012; print contains(a, 2 april 2012)", "1")
runTestCase("a = 2 april 2012, april 2012; print contains(a, 3 april 2012)", "1")
runTestCase("print contains('a', 'aA')", "0")
runTestCase("print contains('ba', 'a')", "1")
runTestCase("print contains('ba', 'A')", "0")
runTestCase("print contains('baos', 'as')", "0")
runTestCase("print contains('baos', 'a0')", "0")
runTestCase("print contains('baos', 'ao')", "1")
runTestCase("print CONTAINS ('baos', 'baos')", "1")
runTestCase("print contains('baos', 'baost')", "0")
runTestCase("print contains('', 'baost')", "0")
runTestCase("print contains('', '')", "1")
runTestCase("print contains('4', '')", "1")
runTestCase("print endswith('a', 'aA')", "0")
runTestCase("print endSWIth('ba', 'a')", "1")
runTestCase("print endswith('ba', 'b')", "0")
runTestCase("print endswith('ba', 'A')", "0")
runTestCase("print endswith('baos', 'os')", "1")
runTestCase("print endswith('baos', 'as')", "0")
runTestCase("print endswith('baos', 'a0')", "0")
runTestCase("print endswith('baos', 'ao')", "0")
runTestCase("print endswith('baos', 'baos')", "1")
runTestCase("print endswith('baos', 'baost')", "0")
runTestCase("print endswith('', 'baost')", "0")
runTestCase("print endswith('', '')", "1")
runTestCase("print endswith('4', '')", "1")
runTestCase("print startsWith('a', 'aA')", "0")
runTestCase("print startswith('ba', 'a')", "0")
runTestCase("print startswith('ba', 'b')", "1")
runTestCase("print startswith('ba', 'B')", "0")
runTestCase("print startswith('ba', 'A')", "0")
runTestCase("print startswith('baos', 'os')", "0")
runTestCase("print startswith('baos', 'as')", "0")
runTestCase("print startswith('baos', 'a0')", "0")
runTestCase("print startswith('baos', 'ao')", "0")
runTestCase("print startswith('baos', 'baos')", "1")
runTestCase("print startswith('baos', 'b')", "1")
runTestCase("print startswith('baos', '')", "1")
runTestCase("print startswith('baos', 'BA')", "0")
runTestCase("print startswith('baos', 'bas')", "0")
runTestCase("print startswith('baos', 'ba')", "1")
runTestCase("print startsWIth('baos', 'baost')", "0")
runTestCase("print startswith('', 'baost')", "0")
runTestCase("print startswith('', '')", "1")
runTestCase("print startswith('4', '')", "1")
runTestCase("print startswith('7cat', length('catara'))", "0")
runTestCase("print startswith('7cat', length('catarac'))", "1")
runTestCase("print startswith('7cat', length('cataract'))", "0")
runTestCase("print isBinary('')", "0")
runTestCase("print isBinary('01')", "1")
runTestCase("print isBinary('011002')", "0")
runTestCase("print isBinary('0110010000')", "1")
runTestCase("print isBinary(' 0110010000')", "0")
runTestCase("print isBinary(' 110010000 ')", "0")
runTestCase("print isBinary('011001a000')", "0")
runTestCase("print isBinary('0000000000')", "1")
runTestCase("print isBinary('11111111111')", "1")
runTestCase("print isBinary('11111I11111')", "0")
runTestCase("print isdigit('')", "0")
runTestCase("print isdigit('1')", "1")
runTestCase("print isdigit('a')", "0")
runTestCase("print isdigit('003h6')", "0")
runTestCase("print isdigit('534')", "1")
runTestCase("print isHex('')", "0")
runTestCase("print isHex('1234')", "1")
runTestCase("print isHex('12v34')", "0")
runTestCase("print isHex('AAbb')", "1")
runTestCase("print isHex('AAEb')", "1")
runTestCase("print isHex('AAGb')", "0")
runTestCase("print isHex('00000')", "1")
runTestCase("print isHex('aa 44')", "0")
runTestCase("print isHex('aa44')", "1")
runTestCase("print isHex('aa44 ')", "0")
runTestCase("print absolute(3)", "3")
runTestCase("print absolute(-4)", "4")
runTestCase("print absolute(-3.2)", "3.2")
runTestCase("print absolute(0)", "0")
runTestCase("print absolute(-1567.76)", "1567.76")
runTestCase("print absolute(2567.76)", "2567.76")
runTestCase("print ceil(3.2)", "4")
runTestCase("print ceil(3)", "3")
runTestCase("print ceil(0.6)", "1")
runTestCase("print ceil(0)", "0")
runTestCase("print ceil(-0)", "0")
runTestCase("print ceil(-0.2)", "0")
runTestCase("print ceil(-23.56)", "-23")
runTestCase("a = 2, 3; print count(a)", "2")
runTestCase("a = 2, 3,1+2; b = 4,6,7,5,7,4; print count(a)", "3")
runTestCase("a = 2, '3', 4; print count(a)", "3")
runTestCase("a = 2, '3', 4; print count(a where this != 3)", "2")
runTestCase("a = 2, '3', 4; print count(a where this = 23)", "0")
runTestCase("a = april 2022, june 2022; print count(a)", "2")
runTestCase("a = april 2022, april 2022; print count(a)", "2")
runTestCase("print floor(1777)", "1777")
runTestCase("print floor(3.2)", "3")
runTestCase("print floor(3)", "3")
runTestCase("print floor(0.6)", "0")
runTestCase("print floor(0)", "0")
runTestCase("print floor(-0)", "0")
runTestCase("print floor(-0.2)", "-1")
runTestCase("print floor(-23.56)", "-24")
runTestCase("print floor(-230)", "-230")
runTestCase("print frombinary('101')", "5")
runTestCase("print frombinary('1')", "1")
runTestCase("print frombinary(101.00)", "5")
runTestCase("print frombinary('101df01')", "0")
runTestCase("print length('abc')", "3")
runTestCase("print length('')", "0")
runTestCase("print length('wo')", "2")
runTestCase("print length('3' + 105)", "4")
runTestCase("print length(10*10)", "3")
runTestCase("print length(1/2)", "3")
runTestCase("print number('4')", "4")
runTestCase("print number('4.32')", "4.32")
runTestCase("print number('4.32kb')", "0")
runTestCase("print number('-123')", "-123")
runTestCase("print number('--123')", "0")
runTestCase("print number('-45.5')", "-45.5")
runTestCase("print number('')", "0")
runTestCase("print number('10k')", "0")
runTestCase("print number('2mb')", "0")
runTestCase("print number('2045')", "2045")
runTestCase("print number(' 2045')", "0")
runTestCase("print number('33 ')", "0")
runTestCase("print number('33')", "33")
runTestCase("print power(2, 4)", "16")
runTestCase("print power(2.5, 4)", "39.0625")
runTestCase("print power(-2.5, 4)", "39.0625")
runTestCase("print power(-2.5, 3)", "-15.625")
runTestCase("print power(-2.5, 0)", "1")
runTestCase("print power(-2.5, 1)", "-2.5")
runTestCase("print power(2, 10)", "1024")
runTestCase("print power(0.5, 2)", "0.25")
runTestCase("print power(16, 0.5)", "4")
runTestCase("print power(16, 0.25)", "2")
runTestCase("print power(16, -0.25)", "0.5")
runTestCase("print power(-16, -1)", "-0.0625")
runTestCase("print power(-16, 1)", "-16")
runTestCase("print round(0)", "0")
runTestCase("print round(3)", "3")
runTestCase("print round(3.14)", "3")
runTestCase("print round(3.5)", "4")
runTestCase("print round(3.7)", "4")
runTestCase("print round(4)", "4")
runTestCase("print round(223)", "223")
runTestCase("print round(4.499999)", "4")
runTestCase("print round(4.500001)", "5")
runTestCase("print round(-4.499999)", "-4")
runTestCase("print round(-4.500001)", "-5")
runTestCase("print round(-0.2)", "0")
runTestCase("print round(-0.5)", "-1")
runTestCase("print round(-1.5)", "-2")
runTestCase("print round(-0.7)", "-1")
runTestCase("print round(-156)", "-156")
runTestCase("print sign(1)", "1")
runTestCase("print sign(13)", "1")
runTestCase("print sign(-23)", "-1")
runTestCase("print sign(0)", "0")
runTestCase("print sign(0.001)", "1")
runTestCase("print sign(-0.0005)", "-1")
runTestCase("print sign(-12.45)", "-1")
runTestCase("print sign(18.4)", "1")
runTestCase("print sqrt(1)", "1")
runTestCase("print sqrt(4)", "2")
runTestCase("print sqrt(16)", "4")
runTestCase("print sqrt(1024)", "32")
runTestCase("print sqrt(0.25)", "0.5")
runTestCase("print sqrt(0.16)", "0.4")
runTestCase("print sqrt(289)", "17")
runTestCase("print sqrt(114.49)", "10.7")
runTestCase("print truncate(4)", "4")
runTestCase("print truncate(0)", "0")
runTestCase("print truncate(132)", "132")
runTestCase("print truncate(-5)", "-5")
runTestCase("print truncate(-375)", "-375")
runTestCase("print truncate(4.2)", "4")
runTestCase("print truncate(34.999)", "34")
runTestCase("print truncate(35.01)", "35")
runTestCase("print truncate(-2.4)", "-2")
runTestCase("print truncate(-3.2)", "-3")
runTestCase("print truncate(-56.2)", "-56")
runTestCase("t = 1,2,3,4,5; average(t), min(t), max(t)", lines("3", "1", "5"))
runTestCase("t = 1,2,3,4,5; sum(t), median(t)", lines("15", "3"))
runTestCase("t = 1,2,3,4,5; sum(t, t), median(t, t)", lines("30", "3"))
runTestCase("t = 1,2,3,4,5; average(t, t, t), min(t,t,t), max(t,t,t)", lines("3", "1", "5"))
runTestCase("t = 34, 13, -8, 15, -12.4, 124; average(t), min(t), max(t)", lines("27.6", "-12.4", "124"))
runTestCase("t = 34, 13, -8, 15, -12.4, 124; sum(t), median(t)", lines("165.6", "14"))
runTestCase("t = 1,2,3,4,5; sum(t, 5), median(6, t, 6), median(6, t, 6, 6)", lines("20", "4", "4.5"))






runTestCase(" 'a.txt' {  exists; size }  ", lines("1", "47"))
runTestCase("countInside(files, 'many texts') ", "30")
runTestCase("inside 'many texts' { count (files) } ", "30")
runTestCase("inside 'many texts' { count (files where extension = 'txt') }", "30")
runTestCase("inside 'many texts' { count (files where extension != 'txt') }", "0")
runTestCase("inside 'many texts' { count (files where extension = 'jpg') }", "0")
runTestCase("inside 'many texts' { size (files) } ", "810")
runTestCase("inside 'many texts' { size (directories) } ", "0")
runTestCase("inside 'many texts' { files order by name asc limit 5 } ", lines("ex_01.txt", "ex_02.txt", "ex_03.txt", "ex_04.txt", "ex_05.txt"))
runTestCase("inside 'many texts' { files order by name asc limit 3 } ", lines("ex_01.txt", "ex_02.txt", "ex_03.txt"))
runTestCase("inside 'many texts' { files order by name desc limit 5 } ", lines("ex_30.txt", "ex_29.txt", "ex_28.txt", "ex_27.txt", "ex_26.txt"))
runTestCase("inside 'many texts' { files order by name desc limit 3 } ", lines("ex_30.txt", "ex_29.txt", "ex_28.txt"))
runTestCase("inside 'many texts' { files order by name desc every 4 limit 5 } ", lines("ex_30.txt", "ex_26.txt", "ex_22.txt", "ex_18.txt", "ex_14.txt"))
runTestCase("inside 'many texts' { files order by name desc skip 2 every 4 limit 5 } ", lines("ex_28.txt", "ex_24.txt", "ex_20.txt", "ex_16.txt", "ex_12.txt"))
runTestCase("inside 'many texts' { files order by name desc skip 1 every 4 limit 5 } ", lines("ex_29.txt", "ex_25.txt", "ex_21.txt", "ex_17.txt", "ex_13.txt"))
runTestCase("inside 'many texts' { files order by name desc every 4 limit 5 skip 1 } ", lines("ex_26.txt", "ex_22.txt", "ex_18.txt", "ex_14.txt"))
runTestCase("inside 'many texts' { files order by name desc where right(name, 1) = 5 } ", lines("ex_25.txt", "ex_15.txt", "ex_05.txt"))
runTestCase("inside 'many texts' { files order by name desc limit 1 { name, fullname, extension } }", lines("ex_30", "ex_30.txt", "txt"))
runTestCase("inside 'many texts' { files order by name desc limit 1 { name(parent) } } ", "many texts")
runTestCase("inside 'many texts' { files order by name desc limit 1 { length(name) } }", "5")
runTestCase("inside 'many texts' { files order by name desc where name[3] = 1 and name[4] = 2 } ", "ex_12.txt")
runTestCase("inside 'many texts' { a= 0; files {if name[3] in 0,3 {a++}}; print a } ", "10")
runTestCase("inside 'modificables' { 'existing_empty_dir' {exists } }", "1")
runTestCase("inside 'modificables' { 'non_existing_empty_dir' {exists } }", "0")
runTestCase("inside 'modificables' { 'existing_empty_dir' {isDirectory } }", "1")
runTestCase("inside 'modificables' { 'existing_empty_dir' {isFile } }", "0")
runTestCase("inside 'modificables' { 'non_existing_empty_dir' {isDirectory } }", "1")
runTestCase("inside 'modificables' { 'non_existing_empty_dir' {isFile } }", "0")
runTestCase("inside 'modificables' { '' {exists, isFile, isDirectory, length(path) } }", lines("0", "0", "0", "0"))
runTestCase("inside 'modificables' { '?/?' {exists, isFile, isDirectory, length(path) } }", lines("0", "0", "0", "0"))
runTestCase("inside 'modificables' { 'non_existing_dir' {exists, isFile, isDirectory } }", lines("0", "0", "1"))
runTestCase("inside 'modificables' { 'non_existing_file.txt' {exists, isFile, isDirectory } }", lines("0", "1", "0"))
runTestCase("inside 'modificables' { create 'existing_empty_dir'}", "Failed to create 'existing_empty_dir'")
runTestCase("inside 'modificables' { force create 'existing_empty_dir'}", "Create directory 'existing_empty_dir'")
# runTestCase("inside 'modificables' { create 'non_existing_empty_dir'; delete 'non_existing_empty_dir'}", \
# lines("Create directory 'non_existing_empty_dir'", "Delete 'non_existing_empty_dir'"))
# runTestCase("inside 'modificables' { recreate 'rainbow.png' to 3 june 2020, 10:11:12 } inside 'modificables' { 'rainbow.png' {print creation}}", \
# lines("Recreate 'rainbow.png' to 3 June 2020, 10:11:12", "3 June 2020, 10:11:12"))
# runTestCase("inside 'modificables' { reaccess 'rainbow.png' to 4 june 2019, 9:05:01 } inside 'modificables' { 'rainbow.png' {print access}}", \
# lines("Reaccess 'rainbow.png' to 4 June 2019, 09:05:01", "4 June 2019, 09:05:01"))
# runTestCase("inside 'modificables' { remodify 'rainbow.png' to 12 april 2017, 14:12:34 } inside 'modificables' { 'rainbow.png' {print modification }}", \
# lines("Remodify 'rainbow.png' to 12 April 2017, 14:12:34", "12 April 2017, 14:12:34"))
# runTestCase("inside 'modificables' { 'a.txt' { print parent(path) = location }}", "1")





expectSyntaxError("print first()")
expectSyntaxError("print first(2,3,4)")
expectSyntaxError("print last()")
expectSyntaxError("print first(2,'fgh')")
expectSyntaxError("print first(56)")
expectSyntaxError("print first(21 april 2007)")
expectSyntaxError("print last(56*4)")
expectSyntaxError("print last('thi')")
expectSyntaxError("print any(5 january 2011, 13:42)")
expectSyntaxError("print any(5 years)")
expectSyntaxError("print any(56)")
expectSyntaxError("print any('56')")
expectSyntaxError("print any('')")
expectSyntaxError("print any()")
expectSyntaxError("a = 2,'aa',4; print contains(a, 'aA', 17)")
expectSyntaxError("a = 2,'aa',4; print contains(a)")
expectSyntaxError("print contains()")
expectSyntaxError("print startswith()")
expectSyntaxError("print startswith(2)")
expectSyntaxError("print startswith(2,3,4)")
expectSyntaxError("print endsWith()")
expectSyntaxError("print endswith(2)")
expectSyntaxError("print ENDSWITH(2,3,4)")
expectSyntaxError("print ENDSWITHs(2,'3')")
expectSyntaxError("print absolute('aa44 ')")
expectSyntaxError("print absolute()")
expectSyntaxError("print ceil(3,6 ,8)")
expectSyntaxError("print ceil('5')")
expectSyntaxError("print ceil(3, 5.3)")
expectSyntaxError("print ceil(april 1996)")
expectSyntaxError("print ceil()")
expectSyntaxError("print count()")
expectSyntaxError("print count(3)")
expectSyntaxError("print count(3 days)")
expectSyntaxError("print count(1 january 2000)")
expectSyntaxError("print count('hey')")
expectSyntaxError("print count('hey', 4)")
expectSyntaxError("print count(2,3,4,5,6)")
expectSyntaxError("print floor(3,6 ,8)")
expectSyntaxError("print floor(3, 5.3)")
expectSyntaxError("print floor(april 1996)")
expectSyntaxError("print floor()")
expectSyntaxError("print length()")
expectSyntaxError("print length(5, 8)")
expectSyntaxError("a = 2,4,5,6; print length(a)")
expectSyntaxError("print number()")
expectSyntaxError("print number('a', 8)")
expectSyntaxError("print number(2,4,5,6)")
expectSyntaxError("print power()")
expectSyntaxError("print power(2)")
expectSyntaxError("print power(2,4,5)")
expectSyntaxError("print power(2,'4')")
expectSyntaxError("print round()")
expectSyntaxError("print round('2')")
expectSyntaxError("print round(2,3)")
expectSyntaxError("print sign()")
expectSyntaxError("print sign(4 years)")
expectSyntaxError("print sign(5, 8.3)")
expectSyntaxError("print sqrt()")
expectSyntaxError("print sqrt(2, 5)")
expectSyntaxError("print sqrt(april 2005)")
expectSyntaxError("print truncate()")
expectSyntaxError("print truncate('truncate')")
expectSyntaxError("print truncate(3, 2*9)")
expectSyntaxError("print average()")
expectSyntaxError("print average('t')")
expectSyntaxError("print average(3,2,4,'g')")
expectSyntaxError("print average(7 days, 8 april 1999)")





print ("BLACK-BOX TESTS END")
print ("All tests have passed successfully if there is no error message above.")
input("Press Enter to continue...")
