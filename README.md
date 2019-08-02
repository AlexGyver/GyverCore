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

First Header | Second Header
------------ | -------------
Content from cell 1 | Content from cell 2
Content in the first column | Content in the second column

## Добавлено
