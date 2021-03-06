#include "DriveTrain.h"
#include "../RobotMap.h"
#include "../Commands/TankDrive.h"

#define max(x, y) (((x) > (y)) ? (x) : (y))

DriveTrain::DriveTrain() : Subsystem("DriveTrain"), left(new CANTalon(LEFTMOTOR)), right(new CANTalon(RIGHTMOTOR)),
							gyro(new ADXRS450_Gyro()) {

	left->SetFeedbackDevice(CANTalon::CtreMagEncoder_Relative);
	left->ConfigEncoderCodesPerRev(360);
	left->SetPosition(0);
	right->SetFeedbackDevice(CANTalon::CtreMagEncoder_Relative);
	right->ConfigEncoderCodesPerRev(360);
	right->SetPosition(0);

	right->SetInverted(true);
	gyro->Reset();
	gyro->Calibrate();
}

void DriveTrain::InitDefaultCommand() {
	// Set the default command for a subsystem here.
	// SetDefaultCommand(new MySpecialCommand());
	SetDefaultCommand(new TankDrive());
}

double DriveTrain::Limit(double num, double max) {
	if (num > max)
		return max;

	if (num < -max)
		return -max;

	return num;
}

void DriveTrain::tankDrive(double leftVal, double rightVal) {
	left->Set(DriveTrain::Limit(leftVal, 1.0));
	right->Set(DriveTrain::Limit(rightVal, 1.0));
}

void DriveTrain::arcadeDrive(double moveVal, double rotateVal) {
	double leftMotorOutput, rightMotorOutput;

	CANTalon::FeedbackDeviceStatus sensorStatus = left->IsSensorPresent(CANTalon::CtreMagEncoder_Relative);
	CANTalon::FeedbackDeviceStatus sensorStatus2 = right->IsSensorPresent(CANTalon::CtreMagEncoder_Relative);

	std::cout << "Left, Right: " << sensorStatus << "   " << sensorStatus2 << std::endl;
	// Standard ArcadeDriveTrain algorithm from Google
	if (moveVal > 0.0) {
		if (rotateVal > 0.0) {
			leftMotorOutput = moveVal - rotateVal;
			rightMotorOutput = max(moveVal, rotateVal);
		} else {
			leftMotorOutput = max(moveVal, -rotateVal);
			rightMotorOutput = moveVal + rotateVal;
		}
	} else {
		if (rotateVal > 0.0) {
			leftMotorOutput = -max(-moveVal, rotateVal);
			rightMotorOutput = moveVal + rotateVal;
		} else {
			leftMotorOutput = moveVal - rotateVal;
			rightMotorOutput = -max(-moveVal, -rotateVal);
		}
	}

	left->Set(DriveTrain::Limit(leftMotorOutput, 0.5));
	right->Set(DriveTrain::Limit(rightMotorOutput, 0.5));
}

double DriveTrain::getAngle() {
	double angle = gyro->GetAngle();
	//-180 to 180
	while (angle > 180) {
		angle -= 360;
	}
	while (angle < -180) {
		angle += 360;
	}
	return angle;
}

void DriveTrain::gyroReset() {
	gyro->Reset();
}

double DriveTrain::leftDistance() { //inches
	double relativePosition = (double) (left->GetPulseWidthPosition() & 0xFFF);
	relativePosition = relativePosition * circumference / 360;
	return relativePosition;
}

double DriveTrain::rightDistance() { //inches
	double relativePosition = (double) (right->GetPulseWidthPosition() & 0xFFF);
	relativePosition = relativePosition * circumference / 360;
	return relativePosition;
}
// Put methods for controlling this subsystem
// here. Call these from Commands.
