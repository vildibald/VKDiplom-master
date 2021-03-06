using System.Collections;
using System.Collections.Generic;
using HermiteInterpolation.Shapes.SplineInterpolation;

namespace HermiteInterpolation.SplineKnots
{
    public delegate KnotMatrix KnotsOperation(
        KnotMatrix leftOp, KnotMatrix rightOp);

    /// <summary>
    ///     Enables chain computation of multiple geneerators.
    ///     Usable for addition/subtraction... of KnotMatrix-es and Spline-s
    /// </summary>
    public sealed class ChainedKnotsGenerator : KnotsGenerator,
        IEnumerable<KnotsGenerator>
    {
        private readonly IList<KnotsGenerator> _generatorChain;
        private readonly IList<KnotsOperation> _operatorChain;

        public ChainedKnotsGenerator(KnotsGenerator first)
        {
            _generatorChain = new List<KnotsGenerator> {first};
            _operatorChain = new List<KnotsOperation> {null};
        }

        public IEnumerator<KnotsGenerator> GetEnumerator()
        {
            return _generatorChain.GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        public override KnotMatrix GenerateKnots(SurfaceDimension uDimension,
            SurfaceDimension vDimension)
        {
            var result = _generatorChain[0].GenerateKnots(uDimension, vDimension);
            for (var i = 1; i < _operatorChain.Count; i++)
            {
                if (_operatorChain[i] == null) continue;
                result = _operatorChain[i](result,
                    _generatorChain[i].GenerateKnots(uDimension, vDimension));
            }
            return result;
        }

        public void Add(KnotsGenerator generator, KnotsOperation operation)
        {
            _generatorChain.Add(generator);
            _operatorChain.Add(operation);
        }
    }
}