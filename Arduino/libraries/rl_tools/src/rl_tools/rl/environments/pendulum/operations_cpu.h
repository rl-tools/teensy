#include "../../../version.h"
#if (defined(RL_TOOLS_DISABLE_INCLUDE_GUARDS) || !defined(RL_TOOLS_RL_ENVIRONMENTS_PENDULUM_OPERATIONS_CPU_H)) && (RL_TOOLS_USE_THIS_VERSION == 1)
#pragma once
#define RL_TOOLS_RL_ENVIRONMENTS_PENDULUM_OPERATIONS_CPU_H

#include "pendulum.h"
#include "operations_generic.h"

#include <string>

RL_TOOLS_NAMESPACE_WRAPPER_START
namespace rl_tools{
    template <typename DEVICE, typename SPEC>
    std::string serialize_json(DEVICE&, rl::environments::Pendulum<SPEC>& env, typename rl::environments::Pendulum<SPEC>::State& state){
        std::string json = "{";
        json += "\"theta\":" + std::to_string(state.theta) + ",";
        json += "\"theta_dot\":" + std::to_string(state.theta_dot);
        json += "}";
        return json;
    }

    template <typename DEVICE, typename SPEC>
    std::string get_ui(DEVICE& device, rl::environments::Pendulum<SPEC>& env){
        std::string ui = R"RL_TOOLS_LITERAL(
function render(ctx, state, action) {
    ctx.clearRect(0, 0, ctx.canvas.width, ctx.canvas.height);

    const centerX = ctx.canvas.width / 2;
    const centerY = ctx.canvas.height / 2;
    const canvasWidth = ctx.canvas.width;

    const pendulumLength = canvasWidth * 0.2;
    const bobRadius = canvasWidth * 0.02;
    const pivotRadius = canvasWidth * 0.01;

    // Draw the Pendulum
    const adjustedTheta = state.theta - Math.PI;

    const pendulumX = centerX + pendulumLength * Math.sin(adjustedTheta);
    const pendulumY = centerY + pendulumLength * Math.cos(adjustedTheta);

    ctx.beginPath();
    ctx.moveTo(centerX, centerY);
    ctx.lineTo(pendulumX, pendulumY);
    ctx.lineWidth = canvasWidth * 0.008;
    ctx.strokeStyle = 'black';
    ctx.stroke();

    ctx.beginPath();
    ctx.arc(pendulumX, pendulumY, bobRadius, 0, 2 * Math.PI);
    ctx.fillStyle = '#7DB9B6';
    ctx.fill();
    ctx.stroke();

    ctx.beginPath();
    ctx.arc(centerX, centerY, pivotRadius, 0, 2 * Math.PI);
    ctx.fillStyle = 'black';
    ctx.fill();
    ctx.stroke();

    // Draw torque arc
    const torqueMagnitude = -Math.max(-1, Math.min(action[0], 1));
    const arrowRadius = canvasWidth * 0.08;
    const magnitudeRadians = (Math.PI * 2 / 3 * torqueMagnitude);
    const startAngle = Math.PI / 2 + (torqueMagnitude > 0 ? 0 : magnitudeRadians);
    const endAngle = Math.PI / 2 + (torqueMagnitude < 0 ? 0 : magnitudeRadians);

    ctx.beginPath();
    ctx.arc(centerX, centerY, arrowRadius, startAngle, endAngle);
    ctx.strokeStyle = 'black';
    ctx.lineWidth = canvasWidth * 0.008;
    ctx.stroke();

    // Draw arrowhead
    const arrowAngle = torqueMagnitude > 0 ? endAngle : startAngle;
    const arrowHeadAngularOffset = torqueMagnitude * Math.PI/180*20
    const arrowX = centerX + arrowRadius * Math.cos(arrowAngle + arrowHeadAngularOffset);
    const arrowY = centerY + arrowRadius * Math.sin(arrowAngle + arrowHeadAngularOffset);

    const headlen = canvasWidth * 0.04 * Math.min(Math.abs(torqueMagnitude)*2, 1);
    const angleOffset = Math.PI / 6;
    const rotationAngle = Math.PI / 2 + (torqueMagnitude > 0 ? 0 : Math.PI);

    ctx.beginPath();
    ctx.moveTo(arrowX, arrowY);
    ctx.lineTo(
        arrowX - headlen * Math.cos(arrowAngle + arrowHeadAngularOffset/2 - angleOffset + rotationAngle),
        arrowY - headlen * Math.sin(arrowAngle + arrowHeadAngularOffset/2 - angleOffset + rotationAngle)
    );
    ctx.lineTo(
        arrowX - headlen * Math.cos(arrowAngle + arrowHeadAngularOffset/2 + angleOffset + rotationAngle),
        arrowY - headlen * Math.sin(arrowAngle + arrowHeadAngularOffset/2 + angleOffset + rotationAngle)
    );
    ctx.lineTo(arrowX, arrowY);
    ctx.fillStyle = 'black';
    ctx.fill();
}
        )RL_TOOLS_LITERAL";
        return ui;
    }
}
RL_TOOLS_NAMESPACE_WRAPPER_END

#endif
