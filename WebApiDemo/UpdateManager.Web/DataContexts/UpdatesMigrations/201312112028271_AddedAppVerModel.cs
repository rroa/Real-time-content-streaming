namespace UpdateManager.Web.DataContexts.UpdatesMigrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class AddedAppVerModel : DbMigration
    {
        public override void Up()
        {
            CreateTable(
                "UpdateManager.ApplicationVersions",
                c => new
                    {
                        Id = c.Int(nullable: false, identity: true),
                        UpdateId = c.Int(nullable: false),
                        CreationDate = c.DateTime(nullable: false),
                    })
                .PrimaryKey(t => t.Id)
                .ForeignKey("UpdateManager.Updates", t => t.UpdateId, cascadeDelete: true)
                .Index(t => t.UpdateId);
            
        }
        
        public override void Down()
        {
            DropForeignKey("UpdateManager.ApplicationVersions", "UpdateId", "UpdateManager.Updates");
            DropIndex("UpdateManager.ApplicationVersions", new[] { "UpdateId" });
            DropTable("UpdateManager.ApplicationVersions");
        }
    }
}
