function plotter()
xyzpos = csvread('hmd-chaipos.csv');
scatter3(xyzpos(:,1), xyzpos(:,2), xyzpos(:,3));

end