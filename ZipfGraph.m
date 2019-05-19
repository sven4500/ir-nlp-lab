filename = uigetfile('*.txt');
fd = fopen(filename, 'r');
if fd <= 0
    disp('Не получилось открыть файл!');
    return;
end
A = fscanf(fd, '%d');
fclose(fd);
X = 1:length(A);
%A=A./max(A);
B = max(A) ./ X;
plot(X,A,X,B);
pbaspect([1 1 1]);
