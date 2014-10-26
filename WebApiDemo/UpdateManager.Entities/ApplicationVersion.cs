using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace UpdateManager.Entities
{
    public class ApplicationVersion
    {
        public int Id { get; set; }

        public int UpdateId { get; set; }

        public int Major { get; set; }
        public int Minor { get; set; }
        public int Patch { get; set; }

        public DateTime CreationDate { get; set; }

        public virtual Update ApplicationUpdate { get; set; }
    }
}
