filename = uigetfile('*.txt');
fd = fopen(filename, 'r');
if fd <= 0
    disp('�� ���������� ������� ����!');
    return;
end
A = fscanf(fd, '%d');
fclose(fd);
X = 1:length(A);
% B - ��������� ��������� �����.
B = max(A) ./ X;
% logb - ������� ���������������� �� ������������� ���������.
logb=@(x,b)(log(x)./log(b));
% C - ����� ������������ ���� M(r)=p*r^(-b).
% m - ���������� ������ �������� ������.
% p - ������������ ������� �����.
Cm = 100;
Cp = A(1);
Cb = -1 .* logb(A(Cm) ./ Cp, Cm);
sprintf('p=%f, b=%f', Cp, Cb)
C = Cp .* X .^ (-Cb);
plot(X,A,X,B,X,C);
pbaspect([1 1 1]);
