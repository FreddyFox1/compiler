         X = 1
         WHILE X<=100 DO
         PRINT X, X/2, X, X*X
         X = X+1
         END
         GOSUB 300
         PRINT "Привет"
         INPUT H
         IF H<11 THEN GOTO 200
         PRINT 12-4/2
         PRINT 100
         200 A = 100/2
         IF A>10 THEN PRINT "Все нормально"
         PRINT A
         PRINT A+34
         INPUT H
         PRINT H
         INPUT "Это тест",y
         PRINT H+Y
         END
         300 PRINT "Это подпрограмма"
             RETURN

