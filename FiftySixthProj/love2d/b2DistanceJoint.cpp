/*
* Copyright (c) 2006-2007 Erin Catto http://www.gphysics.com
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#include "b2DistanceJoint.h"
#include "b2Body.h"
#include "b2World.h"

// 1-D constrained system
// m (v2 - v1) = lambda
// v2 + (beta/h) * x1 + gamma * lambda = 0, gamma has units of inverse mass.
// x2 = x1 + h * v2

// 1-D mass-damper-spring system
// m (v2 - v1) + h * d * v2 + h * k * 

// C = norm(p2 - p1) - L
// u = (p2 - p1) / norm(p2 - p1)
// Cdot = dot(u, v2 + cross(w2, r2) - v1 - cross(w1, r1))
// J = [-u -cross(r1, u) u cross(r2, u)]
// K = J * invM * JT
//   = invMass1 + invI1 * cross(r1, u)^2 + invMass2 + invI2 * cross(r2, u)^2

void b2DistanceJointDef::Initialize(b2Body* b1, b2Body* b2,
									const b2Vec2& anchor1, const b2Vec2& anchor2)
{
	body1 = b1;
	body2 = b2;
	localAnchor1 = body1->GetLocalPoint(anchor1);
	localAnchor2 = body2->GetLocalPoint(anchor2);
	b2Vec2 d = anchor2 - anchor1;
	length = d.Length();
}


b2DistanceJoint::b2DistanceJoint(const b2DistanceJointDef* def)
: b2Joint(def)
{
	m_localAnchor1 = def->localAnchor1;
	m_localAnchor2 = def->localAnchor2;
	m_length = def->length;
	m_frequencyHz = def->frequencyHz;
	m_dampingRatio = def->dampingRatio;
	m_impulse = 0.0f;
	m_gamma = 0.0f;
	m_bias = 0.0f;
	m_inv_dt = 0.0f;
}

void b2DistanceJoint::InitVelocityConstraints(const b2TimeStep& step)
{
	m_inv_dt = step.inv_dt;

	b2Body* b1 = m_body1;
	b2Body* b2 = m_body2;

	// Compute the effective mass matrix.
	b2Vec2 r1 = b2Mul(b1->GetXForm().R, m_localAnchor1 - b1->GetLocalCenter());
	b2Vec2 r2 = b2Mul(b2->GetXForm().R, m_localAnchor2 - b2->GetLocalCenter());
	m_u = b2->m_sweep.c + r2 - b1->m_sweep.c - r1;

	// Handle singularity.
	float32 length = m_u.Length();
	if (length > b2_linearSlop)
	{
		m_u *= 1.0f / length;
	}
	else
	{
		m_u.Set(0.0f, 0.0f);
	}

	float32 cr1u = b2Cross(r1, m_u);
	float32 cr2u = b2Cross(r2, m_u);
	float32 invMass = b1->m_invMass + b1->m_invI * cr1u * cr1u + b2->m_invMass + b2->m_invI * cr2u * cr2u;
	b2Assert(invMass > B2_FLT_EPSILON);
	m_mass = 1.0f / invMass;

	if (m_frequencyHz > 0.0f)
	{
		float32 C = length - m_length;

		// Frequency
		float32 omega = 2.0f * b2_pi * m_frequencyHz;

		// Damping coefficient
		float32 d = 2.0f * m_mass * m_dampingRatio * omega;

		// Spring stiffness
		float32 k = m_mass * omega * omega;

		// magic formulas
		m_gamma = 1.0f / (step.dt * (d + step.dt * k));
		m_bias = C * step.dt * k * m_gamma;

		m_mass = 1.0f / (invMass + m_gamma);
	}

	if (step.warmStarting)
	{
		m_impulse *= step.dtRatio;
		b2Vec2 P = m_impulse * m_u;
		b1->m_linearVelocity -= b1->m_invMass * P;
		b1->m_angularVelocity -= b1->m_invI * b2Cross(r1, P);
		b2->m_linearVelocity += b2->m_invMass * P;
		b2->m_angularVelocity += b2->m_invI * b2Cross(r2, P);
	}
	else
	{
		m_impulse = 0.0f;
	}
}

void b2DistanceJoint::SolveVelocityConstraints(const b2TimeStep& step)
{
	B2_NOT_USED(step);

	b2Body* b1 = m_body1;
	b2Body* b2 = m_body2;

	b2Vec2 r1 = b2Mul(b1->GetXForm().R, m_localAnchor1 - b1->GetLocalCenter());
	b2Vec2 r2 = b2Mul(b2->GetXForm().R, m_localAnchor2 - b2->GetLocalCenter());

	// Cdot = dot(u, v + cross(w, r))
	b2Vec2 v1 = b1->m_linearVelocity + b2Cross(b1->m_angularVelocity, r1);
	b2Vec2 v2 = b2->m_linearVelocity + b2Cross(b2->m_angularVelocity, r2);
	float32 Cdot = b2Dot(m_u, v2 - v1);

	float32 impulse = -m_mass * (Cdot + m_bias + m_gamma * m_impulse);
	m_impulse += impulse;

	b2Vec2 P = impulse * m_u;
	b1->m_linearVelocity -= b1->m_invMass * P;
	b1->m_angularVelocity -= b1->m_invI * b2Cross(r1, P);
	b2->m_linearVelocity += b2->m_invMass * P;
	b2->m_angularVelocity += b2->m_invI * b2Cross(r2, P);
}

bool b2DistanceJoint::SolvePositionConstraints()
{
	if (m_frequencyHz > 0.0f)
	{
		return true;
	}

	b2Body* b1 = m_body1;
	b2Body* b2 = m_body2;

	b2Vec2 r1 = b2Mul(b1->GetXForm().R, m_localAnchor1 - b1->GetLocalCenter());
	b2Vec2 r2 = b2Mul(b2->GetXForm().R, m_localAnchor2 - b2->GetLocalCenter());

	b2Vec2 d = b2->m_sweep.c + r2 - b1->m_sweep.c - r1;

	float32 length = d.Normalize();
	float32 C = length - m_length;
	C = b2Clamp(C, -b2_maxLinearCorrection, b2_maxLinearCorrection);

	float32 impulse = -m_mass * C;
	m_u = d;
	b2Vec2 P = impulse * m_u;

	b1->m_sweep.c -= b1->m_invMass * P;
	b1->m_sweep.a -= b1->m_invI * b2Cross(r1, P);
	b2->m_sweep.c += b2->m_invMass * P;
	b2->m_sweep.a += b2->m_invI * b2Cross(r2, P);

	b1->SynchronizeTransform();
	b2->SynchronizeTransform();

	return b2Abs(C) < b2_linearSlop;
}

b2Vec2 b2DistanceJoint::GetAnchor1() const
{
	return m_body1->GetWorldPoint(m_localAnchor1);
}

b2Vec2 b2DistanceJoint::GetAnchor2() const
{
	return m_body2->GetWorldPoint(m_localAnchor2);
}

b2Vec2 b2DistanceJoint::GetReactionForce() const
{
	b2Vec2 F = (m_inv_dt * m_impulse) * m_u;
	return F;
}

float32 b2DistanceJoint::GetReactionTorque() const
{
	return 0.0f;
}
