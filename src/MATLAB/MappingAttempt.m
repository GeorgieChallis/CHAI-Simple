close all
clear all

xyzfile = csvread('all-swapped2.csv');
xyzfile2 = csvread('all-swapped2.csv');

%Rotation variables
rX = 0;
rY = 0;
rZ = 0;
%Translation Variables
tX = 0;
tY = 0;
tZ = 0;

%Oculus Position
x1 = xyzfile(:,1);
y1 = xyzfile(:,2);
z1 = xyzfile(:,3);
l = length(x1);
homogenous = ones(l, 1);
xyz1 = [x1 y1 z1];


%VICON position
x2 = xyzfile(:,4);
y2 = xyzfile(:,5);
z2 = xyzfile(:,6);
l = length(x1);
xyz2 = [x2 y2 z2];
nPoints = size(xyz2, 1);


% xyz3 = nan(nPoints, 3);
% guessMatrix = zeros(3, 4);
% guessMatrix(1:3, 1:3) = eye(3);
% 
% theta = pi/4.3;
% guessMatrix(1, 1) = cos(theta);
% guessMatrix(1, 2) = -sin(theta);
% guessMatrix(2, 1) = sin(theta);
% guessMatrix(2, 2) = cos(theta);
% fakeMatrix = [guessMatrix; 0.5 -0.76 0.45 1]';
% 
% % Create fake set of coordinates
% for i = 1:nPoints
%     
%     theseCoords = xyz2(i, :);
%     theseCoordsMoved = fakeMatrix * [theseCoords 1]';
%     theseCoordsMoved = theseCoordsMoved(1:3);
% 
%     xyz3(i, :) = theseCoordsMoved(1:3)';
% end



% Guess matrix
guessMatrix = zeros(3, 4);
guessMatrix(1:3, 1:3) = eye(3);


% First recover a mapping with fminunc
options = optimoptions('fminunc');
options.MaxFunctionEvaluations = 100000;
options.MaxIterations = 100000;
options.OptimalityTolerance = 1.0e-20;
options.StepTolerance = 1.0e-20;
[kt, fval] = fminunc(@(k)alignPointsThreeDim(xyz1, xyz2, nPoints, k), guessMatrix, options);

% Recovered matrix
recMat = [kt; 0 0 0 1];
err = fval

% Rotate and translate the points
xyz3m = nan(8, 3);
for i = 1:nPoints
    theseCoords = xyz1(i, :);
    theseCoordsMoved = recMat * [theseCoords 1]';
    xyz3m(i, :) = theseCoordsMoved(1:3);  
end


f = figure;
t = uitable('ColumnName', {'T1', 'T2', 'T3', 'T4'},'units','normalized','Position',[0.2 0.1,0.6,0.22]);
drawnow;
set(t, 'Data', recMat)

subplot(3,1,[1,2])
scatter3(x1, y1, z1, 'g')
hold on
 scatter3(x2, y2, z2, 'r')
 scatter3(xyz3m(:, 1), xyz3m(:, 2), xyz3m(:, 3),'b') 
 xlabel('X');
 ylabel('Y');
 zlabel('Z');
 legend('Oculus XYZ', 'VICON XYZ', 'Transformed Oculus');
 
 
tTemp= annotation('textbox',[0.75 0.58 0.4 0.15],'String',{['RMS Error: '],fval},'FitBoxToText','on');



 hold off
 
 
 return

%Transf Matrix
T = [cos(rY)*cos(rZ) -cos(rY)*sin(rZ) sin(rY) tX;
     cos(rX)*sin(rZ)+sin(rX)*sin(rY)*sin(rZ) cos(rX)*cos(rZ)-sin(rX)*sin(rY)*sin(rZ) -sin(rX)*cos(rY) tY;
     sin(rX)*sin(rZ)-cos(rX)*sin(rY)*cos(rZ) sin(rX)*cos(rZ)+cos(rX)*sin(rY)*sin(rZ) cos(rX)*cos(rY) tZ;
     0 0 0 1];
     
%Transformed Oculus Position
xyz3 = xyz2*T;

diffX = xyz3(:,1)-xyz1(:,1);
diffY = xyz3(:,2)-xyz1(:,2);
diffZ = xyz3(:,3)-xyz1(:,3);

d = sqrt(diffX.^2 + diffY.^2 + diffZ.^2)

% %figure('Name','HMD/VICON Comparison - Step Back');
% subplot(2, 2, 1);
% plot(x1, y1)
% hold on
% plot(x2, y2)
% legend('VICON','Oculus');
% xlabel('X');
% ylabel('Y');
% hold off
% 
% subplot(2, 2, 2);
% plot(x1, z1)
% hold on
% plot(x2, z2)
% legend('VICON','Oculus');
% xlabel('X');
% ylabel('Z');
% hold off
% 
% subplot(2, 2, 3);
% plot(y1, z1)
% hold on
% plot(y2, z2)
% legend('VICON','Oculus');
% xlabel('Y');
% ylabel('Z');
% hold off
% 
% subplot(2, 2, 4);
% plot3(x1, y1, z1)
% hold on
% plot3(x2, y2, z2)
% xlabel('X');
% ylabel('Y');
% zlabel('Z');
% %suptitle('Backwards');
% hold off
% 
% %print('StepBackward','-dpdf')%,'-fillpage')
% 
% 
% %figure();
% %plot(xyzpos(:,7))



