using System.Linq.Expressions;
namespace IoTBackend.Core.Repositories
{
    public interface IRepositoriesBase<T>
    {
        IQueryable<T> FindAll(bool trackChanges);

        T? FindByCondition(Expression<Func<T, bool>> expression, bool trackChanges);

        void Create(T entity);
        void Update(T entity);
        void Remove(T entity);
    }
}