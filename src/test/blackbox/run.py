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
    
#def expectSyntaxError(code):
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











print ("BLACK-BOX TESTS END")
print ("All tests have passed successfully if there is no error message above.")
input("Press Enter to continue...")
