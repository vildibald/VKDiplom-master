﻿using System;
using System.Diagnostics.Contracts;
using System.IO;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

namespace VKDiplom.Utilities.Serialization
{
    public interface ISerializer
    {
        void Serialize<T, TS>(T data, TS stream);

        T Deserialize<T, TS>(TS stream) 
            where TS : Stream;
    }
}