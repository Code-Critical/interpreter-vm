@var 1234            
@sexy 65535             

ldr $01 $00 $00 @sexy
shl $01 $01 $00 10


@loop                
add $02 $02 $00 1    
jlt $02 $01 $00 @loop
str $02 $00 $00 @var
