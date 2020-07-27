:Test
echo Programer
avrdude -P com3 -B 1 -v -c stk500v2 -p m88 -U flash:w:"./Charger.master":i -U lfuse:w:"./lfuse.fus":r -U hfuse:w:"./hfuse.fus":r -U efuse:w:"./efuse.fus":r
pause

