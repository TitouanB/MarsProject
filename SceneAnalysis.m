clear all; close all; clc;

%natural constants:
r2d = 180/pi;
d2r=pi/180;
h=6.6263*10^-34; %Planck's constant
c=3*10^8; %Velocity if light
k=1.38062*10^-23; %Boltzmann's constant
AtargetMin=0.01; % surface's target min [m²]
AtargetMax=0.1; % surface's target max [m²]
shutterTime=10^-3; % shutter time [s]


%Lens system
Dsr=0; %Effective lens entrance aperture
EFL=0; %Effective focal length
alphaLens=0; %Pass ban efficiency of lens systm



IrradianceSun = 530;
Latitude = 50; %latitude in degrees

obliquity = 0;%25.19; ???????????
%at solar noon
angleSunMax = 90-abs(Latitude+obliquity);
IrradianceSunMax=IrradianceSun*sin(angleSunMax*d2r);
%at min work hour
angleSunMin = 10; %arbitrary
IrradianceSunMin=IrradianceSun*sin(angleSunMin*d2r);


%Target properties
alphaMin=0.05; %albedo min
alphaMax=0.45; %albedo max

thetaMin=10; %angle between sun's beam and tagert's normal
thetaMax=50;

%BRDF : 1/10 Glossy and 9/10 Lambertian
RadianceTargetMin = IrradianceSunMin*alphaMin*cos(thetaMax*d2r)*(1/10+9/(10*pi)); % []
RadianceTargetMax = IrradianceSunMax*alphaMax*cos(thetaMin*d2r)*(1/10+9/(10*pi));

LuminousPowerTowardCamMin = RadianceTargetMin*AtargetMin; %[W]
LuminousPowerTowardCamMax = RadianceTargetMax*AtargetMax; %[W]

% Noise photons per shutter time
noiseMin=LuminousPowerTowardCamMin*shutterTime/(h*c/lamdaDEL); %[]




