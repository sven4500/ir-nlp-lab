filename = uigetfile('*.txt');
fd = fopen(filename, 'r');
if fd <= 0
    disp('Не получилось открыть файл!');
    return;
end
A = fscanf(fd, '%d');
fclose(fd);
X = 1:length(A);
% B - некоторый идеальный закон.
B = max(A) ./ X;
% logb - функция логарифмирования по произвольному основанию.
logb=@(x,b)(log(x)./log(b));
% C - закон Мандельброта вида M(r)=p*r^(-b).
% m - количество первых значимых рангов.
% p - максимальная частота слова.
Cm = 100;
Cp = A(1);
Cb = -1 .* logb(A(Cm) ./ Cp, Cm);
sprintf('p=%f, b=%f', Cp, Cb)
C = Cp .* X .^ (-Cb);
plot(X,A,X,B,X,C);
pbaspect([1 1 1]);
