﻿using System;
using HermiteInterpolation.Utils;
using Microsoft.Xna.Framework;

namespace VKDiplom.Engine
{
    public class RotateCamera : Camera
    {
        private float _distance;
        private float _verticalAngle;
        private float _horizontalAngle;
        public float MaxDistance { get; set; }

        public RotateCamera()
        {
            MaxDistance = 100f;
            _distance = 10;
            _verticalAngle = MathHelper.PiOver4;
            _horizontalAngle = MathHelper.PiOver2;
            Position = CoordinateSystems.FromSphericalToCartesian(_distance, _verticalAngle, _horizontalAngle);
        }

        public RotateCamera(float distance, float verticalAngle, float horizontalAngle)
        {
            _distance = distance;
            _verticalAngle = verticalAngle;
            _horizontalAngle = horizontalAngle;
            Position = CoordinateSystems.FromSphericalToCartesian(_distance,_verticalAngle,_horizontalAngle);
        }

        public float Distance
        {
            get { return _distance; }
            set
            {
                _distance = MathHelper.Clamp(value, 0.01f, 100f);
                Position = CoordinateSystems.FromSphericalToCartesian(_distance, _verticalAngle, _horizontalAngle);
            }
        }

        public float VerticalAngle
        {
            get { return _verticalAngle; }
            set
            {
                _verticalAngle = value>MathHelper.Pi?value-MathHelper.TwoPi: value<-MathHelper.Pi? value+MathHelper.TwoPi: value;
                UpVector = _verticalAngle < 0 ? Vector3.Down : Vector3.Up;
                Position = CoordinateSystems.FromSphericalToCartesian(_distance, _verticalAngle, HorizontalAngle);
            }
        }
      
        public float HorizontalAngle
        {
            get { return _horizontalAngle; }
            set
            {
                _horizontalAngle = value % MathHelper.TwoPi;
                Position = CoordinateSystems.FromSphericalToCartesian(_distance, _verticalAngle, _horizontalAngle);
            }
        }

        public Vector3 SphericalPosition
        {
            get { return new Vector3(_distance,_verticalAngle,_horizontalAngle);}
            set
            {
                _distance = value.X;
                _verticalAngle = value.Y;
                _horizontalAngle = value.Z;
                Position = CoordinateSystems.FromSphericalToCartesian(value);
            }
        }
    }
}
