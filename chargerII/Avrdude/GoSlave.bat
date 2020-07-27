:Test
echo Programer
avrdude -P com1 -B 1.1 -v -c stk500v2 -p m88 -U flash:w:"./Charger.slave":i 
avrdude -P com1 -B 1.1 -v -c stk500v2 -p m88 -U lfuse:w:"./lfuse.fus":r -U hfuse:w:"./hfuse.fus":r -U efuse:w:"./efuse.fus":r
pause

