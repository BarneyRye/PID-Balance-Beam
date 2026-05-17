# PID Control Loop for a balancing beam
    - Uses a DC motor to spin a propller which is attached to the end of a beam
    - The goal is to balance the beam at a constant angle, measure by a potentiometer
## Assumptions:
    - Drag/frictional forces are neglected
    - The mass of the beam is negliable
    - Masses are treated as point masses
    - Linearity of equation involving thrust and rpm as below
    - Masses treated as points
    - Infinite acceleration on motor

## Nomenclature:
| Symbol | Meaning | Unit |
| --- | --- | --- |
| F | Force | N |
| ω | Angular Velocity | rad/s |
| V | Voltage | V |
| T | Torque | Nm |
| x | Distance | m |
| α | Angular Acceleration | rad/s^2 |
| I | 2nd Moment of Inertia | kgm^2 |
| m | Mass | kg |
| g | Acceleration due to Gravity (9.81) | m/s^2 |
| θ | Angle | rads |


## Force equations:
$$ F_{thrust} ∝ \omega^2 $$ 
$$ \omega ∝ V $$
$$ \therefore F_{thrust} ∝ V^2 = kV^2 $$

We want to convert the thrust to angular acceleration using:
$$ T = F_{Perpendicular}x  $$
$$ \alpha =  T / I $$ 
However, we must account for mass of the motor and propeller only
$$ F_{Weight} = mg $$
To get the perpendicualr force we must use
$$ F_{Perpendiclar} = |F_{Weight}cos(\theta)| $$
Where theta is the angle to the horizontal, anbd the forcxe allways acts downwards
$$ I = mx^2 $$

## Finally we get:
$$ F_{Total} = F_{Thrust} - F_{Weight}cos(\theta) $$
$$ \alpha = F_{Total}x/I$$

## But we want the angle:
So integrating twice with respect time(t) we get:
$$ \theta = at^2 + bt + c $$
Where a = Fx/2I, b is the initial velocity and c is the initial position. <br>
If we assume its at rest, at the horizontal, we get:
$$ \theta_0 = 0, \omega_0 = 0$$
$$ \theta = (F_{Total}x/2I)t^2 $$

Using small angle approximations:
$$ cos(\theta) = 1$$
We end up with:
---
$$ \theta = (kV^2 - mg)/(2mx) t^2 $$
---

For control loops, we want it in terms of the input, V, giving <br>
**Open Loop Relationship:**
$$ V = \sqrt{(((2mx\theta)/t^2)+mg)/k} $$
*N.B. The value k represents a constant depednding on the motor and propeller characteristics*

# Open loops transfer function:
Going back, if we incorperated everything before the integration, it would leave us with:
$$ \ddot\theta = (kV^2 - mg)/mx $$
Lineraising around a voltage that holds the bar horizontal ie:
$$ kV_0^2 = mg $$
$$ kV^2 = k(V_0 + dV)^2 = kV_0^2 + 2kV_0dV + kdv^2 $$
It becomes, assuming dV is small:
$$ \ddot\theta = 2kV_0dV/mx $$

**Applying a laplace with the same 0 initial conditions we get:**
$$ S^2\theta(s) = (2kV_0/mx) * dV(s) $$

## Open loops transfer function is:
$$ G(s) = \theta(s)/dv(s) = (2kV_0)/(mxs^2)$$
Simplified:
$$ G(s) = a/s^2  $$
$$ a = 2kV_0/mx$$
Where a is a constant depending on the motor and propeller characteristics

# Estimations for simulations
As
$$  kV_0^2 = mg $$
$$ k = mg/V_0^2 $$
Assuming m=100g, x = 20cm, and running on a 6v at 50% duty cycle give V_avg as 3v,
$$ k = 0.982/3^2 = 0.109 $$
$$ \therefore a = (2*0.109*3)/(0.1*0.2) = 32.7 rads/s^2/V $$
Giving:
$$ G(s) = 32.7/s^2 $$

Which has been modelled in the simulink file

