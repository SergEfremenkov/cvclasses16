clear
clc
close all

fileName = './DSobelVsLoG/measures.txt';

fId = fopen(fileName, 'rt');

if fId == -1 
    error('File is not opened'); 
end 

[value, ~] = fscanf(fId,'%d %f %f %f %d %d %d\n', [7 14]);

fclose(fId);


f1 = figure();
hold on
grid on
xlabel('Precision')
ylabel('Recall')
title('Recall / Precision')

for i = 1:size(value,2)
    if(value(1,i) == 99)
        index = i;
        break;
    end
end

plot(value(3,1:index), value(4,1:index),'-*r');
plot(value(3,(index + 1):end), value(4,(index + 1):end),'-*');
plot(value(3,6), value(4,6),'or');
plot(value(3,7), value(4,7),'ob');

legend('Diagonal Sobel 3x3','LoG 3x3','Последняя точка Sobel','Последняя точка LoG')

 print(f1, '-dpng', 'grafik');