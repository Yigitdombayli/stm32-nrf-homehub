using System.Linq.Expressions;
using IoTBackend.Core.Repositories;
using Microsoft.EntityFrameworkCore;

namespace IoTBackend.Repository
{
        public abstract class RepositoryBase<T> : IRepositoriesBase<T> where T : class, new()
    {
        protected readonly AppDbContext _context;

        protected RepositoryBase(AppDbContext context)
        {
            _context = context;
        }

        public void Create(T entity)
        {
            _context.Set<T>().Add(entity);
        }

        public void Update(T entity) => _context.Set<T>().Update(entity);

        public IQueryable<T> FindAll(bool trackChanges)
        {
             return trackChanges

            ? _context.Set<T>()

            : _context.Set<T>().AsNoTracking();
        }

        public T? FindByCondition(Expression<Func<T, bool>> expression, bool trackChanges)
        {
            return trackChanges
                ? _context.Set<T>().Where(expression).SingleOrDefault()
                : _context.Set<T>().Where(expression).AsNoTracking().SingleOrDefault(); 
        }

        public void Remove(T entity)
        {
            _context.Set<T>().Remove(entity);
        }
    }
}