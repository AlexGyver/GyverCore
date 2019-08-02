# GyverCore for ATmega328p/168p
 Быстрое ядро для Arduino IDE. **В разработке**  
 Основано на оригинальном ядре Arduino версии 1.8.9, большинство функций заменены на более быстрые и лёгкие аналоги, убрано всё лишнее и не относящееся к микроконтроллеру ATmega328p, убран почти весь Wiring-мусор, код упрощён и причёсан. 
# Изменения
## Облегчено и ускорено
Время выполнения функций, мкс

Функция         | Arduino   | GyverCore | Быстрее в 
----------------|-----------|-----------|----------
pinMode         | 2.90 us   | 0.57 us   | 5.09      
digitalWrite    | 2.90 us   | 0.57 us   | 5.09      
digitalRead     | 2.45 us   | 0.50 us   | 4.90      
analogWrite     | 4.15 us   | 1.13 us   | 3.67      
analogRead      | 112.01 us | 5.41 us   | 20.70     
analogReference | 0.00 us   | 0.69 us   | 0.00      
attachInterrupt | 1.20 us   | 1.18 us   | 1.02      
detachInterrupt | 0.82 us   | 0.57 us   | 1.44      

Занимаемое место, Flash

Функция         | Arduino | GyverCore | Разница, Flash 
----------------|---------|-----------|---------------
pinMode         | 114     | 24        | 90             
digitalWrite    | 200     | 24        | 176            
digitalRead     | 190     | 24        | 166            
analogWrite     | 406     | 48        | 358            
analogRead      | 32      | 72        | -40            
analogReference | 0       | 32        | -32            
attachInterrupt | 212     | 180       | 32             
detachInterrupt | 198     | 150       | 48             
Serial.begin    | 1028    | 166       | 862            
print long      | 1094    | 326       | 768            
print string    | 2100    | 1484      | 616            
print float     | 2021    | 446       | 1575           
parseInt        | 1030    | 214       | 816            
readString      | 2334    | 1594      | 740            
parseFloat      | 1070    | 246       | 824                   

## Добавлено
