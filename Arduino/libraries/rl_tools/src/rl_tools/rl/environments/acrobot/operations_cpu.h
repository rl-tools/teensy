#include "../../../version.h"
#if (defined(RL_TOOLS_DISABLE_INCLUDE_GUARDS) || !defined(RL_TOOLS_RL_ENVIRONMENTS_ACROBOT_OPERATIONS_CPU_H)) && (RL_TOOLS_USE_THIS_VERSION == 1)
#pragma once
#define RL_TOOLS_RL_ENVIRONMENTS_ACROBOT_OPERATIONS_CPU_H

#include "acrobot.h"
#include "operations_generic.h"

#include <string>

RL_TOOLS_NAMESPACE_WRAPPER_START
namespace rl_tools{
    template <typename DEVICE, typename SPEC>
    std::string json(DEVICE&, const rl::environments::Acrobot<SPEC>& env, const typename rl::environments::Acrobot<SPEC>::Parameters& parameters){
        return "{}";
    }
    template <typename DEVICE, typename SPEC>
    std::string json(DEVICE&, const rl::environments::Acrobot<SPEC>& env, const typename rl::environments::Acrobot<SPEC>::Parameters& parameters, const typename rl::environments::Acrobot<SPEC>::State& state){
        std::string json = "{";
        json += "\"theta1\":" + std::to_string(state.theta_1) + ",";
        json += "\"theta2\":" + std::to_string(state.theta_2) + ",";
        json += "\"theta1_dot\":" + std::to_string(state.theta_1_dot) + ",";
        json += "\"theta2_dot\":" + std::to_string(state.theta_2_dot);
        json += "}";
        return json;
    }
    template <typename DEVICE, typename SPEC>
    std::string json(DEVICE& device, const rl::environments::AcrobotSwingup<SPEC>& env, const typename rl::environments::Acrobot<SPEC>::Parameters& parameters, const typename rl::environments::Acrobot<SPEC>::State& state){
        return json(device, static_cast<const rl::environments::Acrobot<SPEC>&>(env), parameters, state);
    }
    template <typename DEVICE, typename SPEC, typename PARAM_SPEC>
    std::string json(DEVICE& device, const rl::environments::Acrobot<SPEC>& env, const typename rl::environments::acrobot::Parameters<PARAM_SPEC>& parameters){
        return "{}";
    }

    template <typename DEVICE, typename SPEC>
    std::string get_ui(DEVICE& device, rl::environments::Acrobot<SPEC>& env){
        std::string ui = R"RL_TOOLS_LITERAL(
    ctx.clearRect(0, 0, ctx.canvas.width, ctx.canvas.height);

    const centerX = ctx.canvas.width / 2;
    const centerY = ctx.canvas.height / 2;
    const canvasWidth = ctx.canvas.width;

    const pendulumLength1 = canvasWidth * 0.2;
    const pendulumLength2 = canvasWidth * 0.2;
    const bobRadius = canvasWidth * 0.02;
    const pivotRadius = canvasWidth * 0.01;

    // First pendulum (base to joint)
    const pendulumX1 = centerX + pendulumLength1 * Math.sin(state.theta1);
    const pendulumY1 = centerY + pendulumLength1 * Math.cos(state.theta1);

    ctx.beginPath();
    ctx.moveTo(centerX, centerY);
    ctx.lineTo(pendulumX1, pendulumY1);
    ctx.lineWidth = canvasWidth * 0.008;
    ctx.strokeStyle = 'black';
    ctx.stroke();

    ctx.beginPath();
    ctx.arc(centerX, centerY, pivotRadius, 0, 2 * Math.PI);
    ctx.fillStyle = 'black';
    ctx.fill();
    ctx.stroke();

    // Second pendulum (joint to end)
    const pendulumX2 = pendulumX1 + pendulumLength2 * Math.sin(state.theta1 + state.theta2);
    const pendulumY2 = pendulumY1 + pendulumLength2 * Math.cos(state.theta1 + state.theta2);

    ctx.beginPath();
    ctx.moveTo(pendulumX1, pendulumY1);
    ctx.lineTo(pendulumX2, pendulumY2);
    ctx.lineWidth = canvasWidth * 0.008;
    ctx.strokeStyle = 'black';
    ctx.stroke();

    ctx.beginPath();
    ctx.arc(pendulumX1, pendulumY1, pivotRadius, 0, 2 * Math.PI);
    ctx.fillStyle = 'black';
    ctx.fill();
    ctx.stroke();

    ctx.beginPath();
    ctx.arc(pendulumX2, pendulumY2, bobRadius, 0, 2 * Math.PI);
    ctx.fillStyle = '#7DB9B6';
    ctx.fill();
    ctx.stroke();

    // Draw torque arc
    const torqueMagnitude = -action[0];
    const arrowRadius = canvasWidth * 0.08;
    const magnitudeRadians = (Math.PI * 2 / 3 * torqueMagnitude);
    const startAngle =-Math.PI/2 + (torqueMagnitude > 0 ? 0 : magnitudeRadians) - state.theta1;
    const endAngle = -Math.PI/2 + (torqueMagnitude < 0 ? 0 : magnitudeRadians) - state.theta1;

    ctx.beginPath();
    ctx.arc(pendulumX1, pendulumY1, arrowRadius, startAngle, endAngle);
    ctx.strokeStyle = 'black';
    ctx.lineWidth = canvasWidth * 0.008;
    ctx.stroke();

    // Draw arrowhead
    const arrowAngle = torqueMagnitude > 0 ? endAngle : startAngle;
    const arrowHeadAngularOffset = torqueMagnitude * Math.PI / 180 * 20;
    const arrowX = pendulumX1 + arrowRadius * Math.cos(arrowAngle + arrowHeadAngularOffset);
    const arrowY = pendulumY1 + arrowRadius * Math.sin(arrowAngle + arrowHeadAngularOffset);

    const headlen = canvasWidth * 0.04 * Math.min(Math.abs(torqueMagnitude) * 2, 1);
    const angleOffset = Math.PI / 6;
    const rotationAngle = Math.PI / 2 + (torqueMagnitude > 0 ? 0 : Math.PI);

    ctx.beginPath();
    ctx.moveTo(arrowX, arrowY);
    ctx.lineTo(
        arrowX - headlen * Math.cos(arrowAngle + arrowHeadAngularOffset / 2 - angleOffset + rotationAngle),
        arrowY - headlen * Math.sin(arrowAngle + arrowHeadAngularOffset / 2 - angleOffset + rotationAngle)
    );
    ctx.lineTo(
        arrowX - headlen * Math.cos(arrowAngle + arrowHeadAngularOffset / 2 + angleOffset + rotationAngle),
        arrowY - headlen * Math.sin(arrowAngle + arrowHeadAngularOffset / 2 + angleOffset + rotationAngle)
    );
    ctx.lineTo(arrowX, arrowY);
    ctx.fillStyle = 'black';
    ctx.fill();
        )RL_TOOLS_LITERAL";
        return ui;
    }
    template <typename DEVICE, typename SPEC>
    std::string get_ui(DEVICE& device, rl::environments::AcrobotSwingup<SPEC>& env){
        return get_ui(device, static_cast<rl::environments::Acrobot<SPEC>&>(env));
    }
}
RL_TOOLS_NAMESPACE_WRAPPER_END

#endif
