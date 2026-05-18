close all;
clear;
clc;

Ki = 0;
Kp = 50;
Kd = 10;

Cs = pid(Kp,Ki,Kd);
Gs = tf(32.7, [1, 0, 0]);

Wc = Cs*Gs;

figure;
pzmap(Wc);
title('Pole-Zero Map');
grid on;

figure;
rlocus(Wc);
title('Root Locus Plot');
grid on;

figure;
margin(Wc);
grid on;